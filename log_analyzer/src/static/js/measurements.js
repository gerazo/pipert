import { add_dataset, create_line_live_chart, update } from './charts.js';
import {get_channel_names, get_field} from './utils.js'

let socket = io.connect('http://' + document.domain + ':' + location.port);

let channel_measure_charts = []
let pipeline_measure_charts = []

for (const measure of channels_measurements){
    channel_measure_charts.push(create_line_live_chart(measure[0], measure[1]));
}

for (const measure of pipeline_measurements){
    let chart = create_line_live_chart(measure[0], measure[1]);
    add_dataset(chart, 'Pipeline');
    pipeline_measure_charts.push(chart);
}

let label_begin_channels = 1;
socket.on('measures_update', function(dict){
    channels_measurements.forEach((measure, i) => {
        var rates = get_field(dict, measure[1]);
        var chart = channel_measure_charts[i];
        var channels_name = get_channel_names(dict);
        if (!( chart.data.datasets.length == channels_name.length)) {
            for (const channel_name of channels_name) {
                add_dataset(chart, channel_name);
            }
        }
    
        update(chart, rates, label_begin_channels);
    });
    
    label_begin_channels += 10;
});

let label_begin_pipeline = 1;

socket.on('pipeline_measures_update', function(dict){
    pipeline_measurements.forEach((measure, i) => {
        var chart = pipeline_measure_charts[i];
        var rates = [dict[measure[1]]];
        // console.log(rates);
        update(chart, rates, label_begin_pipeline);
    });
    label_begin_pipeline += 10
});