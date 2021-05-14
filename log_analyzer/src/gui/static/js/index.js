import { create_channels } from './channels.js';
import { create_map } from './channels_map.js'
import { add_dataset, create_line_live_chart, update } from './charts.js';
import {get_channel_names, get_field} from './utils.js'

/*
* Creating the index page's content
* */

let socket = io.connect('http://' + document.domain + ':' + location.port);
let names = [];
let ex_chart = create_line_live_chart("execution_time_chart", "Execution Time");
let drop_chart = create_line_live_chart("drop_rate_chart", "Drop Rate");
let label_begin = 1;
socket.on('update_channels', function(channels){
    create_channels(channels);
});
socket.on('channels_map', function(dict){
    create_map(dict);
});
socket.on('measures_update', function(dict){
    const execution_rates = get_field(dict, "Execution Time");
    const drop_rates = get_field(dict, "Drop Rate");
    if (!( names.length == get_channel_names(dict).length)) {
        names = get_channel_names(dict);
        for (const channel_name of names) {
            add_dataset(ex_chart, channel_name);
            add_dataset(drop_chart, channel_name);
        }
    }
    
    update(ex_chart, execution_rates, label_begin);
    update(drop_chart, drop_rates, label_begin);
    label_begin += 10;
});
