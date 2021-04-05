import { create_channels } from './channels.js';
import { get_channel_color } from './utils.js';

$(document).ready(function(){
    let socket = io.connect('http://' + document.domain + ':' + location.port);
    let execution_time_chart = draw_chart("Execution Time", 
                                          "execution_time_chart", "line", 
                                          [], []);
    let drop_rate_chart = draw_chart("Drop Rates", "drop_rate_chart", 
                                     "horizontalBar", [], []);
    let i = 0;
    socket.on('update_channels', function(channels){
        create_channels(channels);
        var channel_names = get_channel_names(channels);
        var drop_rates = get_field(channels, "DROP_RATE");
        var execution_rates = get_field(channels, "EXECUTION_TIME");
        if (i % 20 == 0) {
            update_chart(drop_rate_chart, channel_names, drop_rates);
            update_chart(execution_time_chart, channel_names, execution_rates);
        }
        i++;
    });
    socket.on('channels_map', function(channels){
        var nodes = create_nodes(channels);
        var edges = create_edges(channels);
        var container = document.getElementById("channels-map");
        var data = {
            nodes: nodes,
            edges: edges
        };
        var options = {};
        var network = new vis.Network(container, data, options);
   });
});

function draw_chart(name, id, chart_type, channel_names, data){
    let myChart = document.getElementById(id).getContext('2d');
    let background_color = undefined;
    if (chart_type == 'line') {
        background_color = 'rgba(0, 0, 0, 0.1)'; 
    }else {
        background_color = [
              'rgba(255, 99, 132, 0.6)',
              'rgba(54, 162, 235, 0.6)',
              'rgba(255, 206, 86, 0.6)',
              'rgba(75, 192, 192, 0.6)',
              'rgba(153, 102, 255, 0.6)',
              'rgba(255, 159, 64, 0.6)',
              'rgba(255, 99, 132, 0.6)'
            ]
    }
    // Chart.defaults.global.defaultFontFamily = 'Lato';
    // Chart.defaults.global.defaultFontSize = 18;
    // Chart.defaults.global.defaultFontColor = '#777';

    let chart = new Chart(myChart, {
        type:chart_type, // bar, horizontalBar, pie, line, doughnut, 
                             // radar, polarArea
        data:{
          labels:channel_names,
          datasets:[{
            label: name,
            data: data,
            backgroundColor: background_color,
            borderWidth:1,
            borderColor:'#777',
            hoverBorderWidth:3,
            hoverBorderColor:'#000'
          }]
        },
        options:{
          title:{
            display:true,
            text:name,
            fontSize:25
          },
          legend:{
            display:false,
          }
          },
          tooltips:{
            enabled:false
          },
          animation: {
            duration: 0 // general animation time
        },
          hover: {
            animationDuration: 0 // duration of animations when hovering an item
          },
          responsiveAnimationDuration: 0, // animation duration after a resize
    });

    return chart;
}

function get_channel_names(channels){
    let names = []
    channels.forEach(function(channel){
        names.push(channel.name);
    });
    
    return names
}

function get_field(channels, field){
    let rates = []
    channels.forEach(function(channel){
        rates.push(channel[field]);
    });
    
    return rates
}

function update_chart(chart, labels, data) {
    chart.data.datasets[0].data = data;
    chart.data.labels = labels;
    chart.update();
}

function create_nodes(arr) {
    let ret = [];
    arr.forEach(function(item, index, array) {
        ret.push({id: index, value:12, shape: "box",
                  color: get_channel_color(item), label: item});
    })

    return ret;
}

function create_edges(arr) {
    let ret = [];
    for (let i = 0; i < (arr.length - 1); i++) {
        ret.push({from: i, to: i+1, arrows: "to"});
    }

    return ret;
}
