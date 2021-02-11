#include "pipert/Channel.h"
#include "pipert/Packet.h"
#include "pipert/Scheduler.h"
#include "pipert/Timer.h"
#include "pipert/Profiler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>

#include <chrono>

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include<opencv2/opencv.hpp>
#include <unistd.h>
#include <string>

#include <iostream>
#include <algorithm>
#include <thread>
#include <string>
#include <unistd.h>

class Human {
 public:
  Human(const std::string& name, int age) : name_(name), age_(age) {}

  std::string GetName() const { return name_; }
  int GetAge() const { return age_; }

 private:
  std::string name_;
  int age_;
};

class Yolo
{
  public:
    Yolo(pipert::PolledChannel<std::vector<cv::Rect>>* pc_to_write) : pc_to_write_(pc_to_write) {}

    // confidence threshold
    float conf_threshold = 0.8;
    // nms threshold
    float nms = 0.6;
    int width = 416;
    int height = 416;

    std::vector<std::string> classes;

    std::vector<cv::Mat> outs;
    cv::dnn::Net net;
    cv::Mat blob;

    //Camera 
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API

    // remove unnecessary bounding boxes
    std::vector<cv::Rect> remove_box(cv::Mat &frame, const std::vector<cv::Mat>&out, cv::dnn::Net *pnet);

    // draw bounding boxes
    void draw_box(int classId, float conf, int left, int top, int right, int bottom, cv::Mat& frame);

    // get output layers
    std::vector<std::string> getOutputsNames(const cv::dnn::Net& net);

    void Detection(pipert::PacketToProcess<cv::Mat> p){
      auto frame = p.data();

        // convert image to blob
      cv::dnn::blobFromImage(frame, blob, 0.00392, cv::Size(width,height),cv::Scalar(0,0,0), true, false);
      net.setInput(blob);

      std::vector<cv::Mat> outs;
      net.forward(outs,getOutputsNames(net));

      std::vector<cv::Rect> g_boxes = remove_box(frame, outs, &net);
      pipert::PacketToFill<std::vector<cv::Rect>> packet_to_fill = pc_to_write_->Acquire(p.timestamp(), g_boxes);
      
      packet_to_fill.Push();
    }
  private:
    pipert::PolledChannel<std::vector<cv::Rect>>* pc_to_write_;
};


int main(int argc, char** argv){
  pipert::Profiler profiler("file:emptylog.txt", 100);

  pipert::Scheduler sch;
  const int channel_capacity = 10;

  pipert::PolledChannel<std::vector<cv::Rect>> pc =
      sch.CreatePolledChannel<std::vector<cv::Rect>>("OutChannel", channel_capacity);

  Yolo yolo(&pc);
  std::string classesFile = "./yolo/cfg/coco.names";
  std::ifstream ifs(classesFile.c_str());
  std::string line;
  while (getline(ifs, line)) yolo.classes.push_back(line);
  // load model weights and architecture
  std::string configuration = "./yolo/cfg/yolov3-tiny.cfg";
  std::string model = "./yolo/cfg/yolov3-tiny.weights";
  // Load the network
  yolo.net = cv::dnn::readNetFromDarknet(configuration, model);
  
  pipert::ScheduledChannel<cv::Mat> sc1 =
      sch.CreateScheduledChannel<cv::Mat>("YoloChannel", channel_capacity, nullptr,
                                        std::bind(&Yolo::Detection, &yolo, std::placeholders::_1));

  //--- INITIALIZE VIDEOCAPTURE
  cv::VideoCapture cap;
  cv::Mat frame;
  cap.open(argv[1]);
  

  sch.Start();

  int const sleepTime = 10000;
  float fps = sleepTime/100.;
  while(true) {
    cap.read(frame);

    // check if we succeeded
    if (frame.empty()) 
        break;
    
    for(pipert::PacketToProcess<std::vector<cv::Rect>> packet_to_process = pc.Poll();
      !packet_to_process.IsEmpty();
      packet_to_process = pc.Poll()) {

        std::vector<cv::Rect> g_boxes = packet_to_process.data();
        std::cout << g_boxes.size()<<'\n';

        int size = g_boxes.size();
        size = cv::min(size,15);
        for (int i = 0; i < size; ++i)
        {
          cv::Rect box = g_boxes[i];
          int left = box.x;
          int top = box.y;
          int right = box.x + box.width;
          int bottom = box.y + box.height;
          cv::rectangle(frame, cv::Point(left, top), cv::Point(right, bottom), 
            cv::Scalar(255, 178, 50), 3);
        }

        packet_to_process.Release();
      }

    pipert::Timer::Time time = pipert::Timer::time();
    pipert::PacketToFill<cv::Mat> packet_to_fill = sc1.Acquire(time, frame);

    // packet_to_fill.Push();

    std::string label = cv::format("FPS : %.2f", fps);
    cv::putText(frame, label, cv::Point(0, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255));
    cv::imshow("Live",frame);
    
    //wait esc
    char c=(char)cv::waitKey(25);
    if(c==27)
        break;
    
    usleep(sleepTime);
    
  }

  usleep(1000);

  std::cout << "END" << '\n'<<'\n';

  sch.Stop();

  return 0;
}


std::vector<cv::Rect> Yolo::remove_box(cv::Mat& frame, const std::vector<cv::Mat>& outs, cv::dnn::Net *pnet)
{

  std::vector<int> classIds;
  std::vector<float> confidences;
  std::vector<cv::Rect> boxes;
  
  static std::vector<int> outLayers = pnet->getUnconnectedOutLayers();
  static std::string outLayerType = pnet->getLayer(outLayers[0])->type;

  if (outLayerType == "DetectionOutput")
  {
    // Network produces output blob with a shape 1x1xNx7 where N is a number of
    // detections and an every detection is a vector of values
    // [batchId, classId, confidence, left, top, right, bottom]
    CV_Assert(outs.size() > 0);
    for (size_t k = 0; k < outs.size(); k++)
    {
      float* data = (float*)outs[k].data;
      for (size_t i = 0; i < outs[k].total(); i += 7)
      {
        float confidence = data[i + 2];
        if (confidence > conf_threshold)
        {
          int left = (int)data[i + 3];
          int top = (int)data[i + 4];
          int right = (int)data[i + 5];
          int bottom = (int)data[i + 6];
          int width = right - left + 1;
          int height = bottom - top + 1;
          if (width * height <= 1)
          {
            left = (int)(data[i + 3] * frame.cols);
            top = (int)(data[i + 4] * frame.rows);
            right = (int)(data[i + 5] * frame.cols);
            bottom = (int)(data[i + 6] * frame.rows);
            width = right - left + 1;
            height = bottom - top + 1;
          }
          classIds.push_back((int)(data[i + 1]) - 1);  // Skip 0th background class id.
          boxes.push_back(cv::Rect(left, top, width, height));
          confidences.push_back(confidence);
        }
      }
    }
  }
  else if (outLayerType == "Region")
  {
    for (size_t i = 0; i < outs.size(); ++i)
    {
      // Network produces output blob with a shape NxC where N is a number of
      // detected objects and C is a number of classes + 4 where the first 4
      // numbers are [center_x, center_y, width, height]
      float* data = (float*)outs[i].data;
      for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
      {
        cv::Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
        cv::Point classIdPoint;
        double confidence;
        cv::minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);

        if (confidence > conf_threshold)
        {
          int centerX = (int)(data[0] * frame.cols);
          int centerY = (int)(data[1] * frame.rows);
          int width = (int)(data[2] * frame.cols);
          int height = (int)(data[3] * frame.rows);
          int left = centerX - width / 2;
          int top = centerY - height / 2;
          classIds.push_back(classIdPoint.x);
          confidences.push_back((float)confidence);
          boxes.push_back(cv::Rect(left, top, width, height));
        }
      }
    }
  }
  else
    CV_Error(cv::Error::StsNotImplemented, "Unknown output layer type: " + outLayerType);

  // Perform non maximum suppression to eliminate redundant overlapping boxes with
    // lower confidences
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, conf_threshold, nms, indices);
    

    std::vector<cv::Rect> g_boxes;

    for (size_t i = 0; i < indices.size(); ++i)
    {
        int idx = indices[i];
        cv::Rect box = boxes[idx];
        g_boxes.push_back(box);

        // draw_box(classIds[idx], confidences[idx], box.x, box.y,
        //          box.x + box.width, box.y + box.height, frame);
    }
    return g_boxes;
}

// Draw the predicted bounding box
void Yolo::draw_box(int classId, float conf, int left, int top, int right, int bottom, cv::Mat& frame)
{
    //Draw a rectangle displaying the bounding box
    // rectangle(frame, Point(left, top), Point(right, bottom), Scalar(255, 178, 50), 3);
    
    //Get the label for the class name and its confidence
    // std::string label = cv::format("%.2f", conf);
    // if (!classes.empty())
    // {
    //     CV_Assert(classId < (int)classes.size());
    //     label = classes[classId] + ":" + label;
    // }

    // std::cout << label << '\n';
    
    //Display the label at the top of the bounding box
    // int baseLine;
    // cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
    // top = cv::max(top, labelSize.height);
    // rectangle(frame, Point(left, top - round(1.5*labelSize.height)), Point(left + round(1.5*labelSize.width), top + baseLine), 
    //   Scalar(255, 255, 255), FILLED);
    // putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,0),1);
}

// Get the names of the output layers

std::vector<std::string> Yolo::getOutputsNames(const cv::dnn::Net& net)
{
    static std::vector<std::string> names;
    if (names.empty())
    {
        //Get the indices of the output layers, i.e. the layers with unconnected outputs
        std::vector<int> outLayers = net.getUnconnectedOutLayers();
        
        //get the names of all the layers in the network
        std::vector<std::string> layersNames = net.getLayerNames();
        
        // Get the names of the output layers in names
        names.resize(outLayers.size());
        for (size_t i = 0; i < outLayers.size(); ++i)
        names[i] = layersNames[outLayers[i] - 1];
    }
    return names;
}