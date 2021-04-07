import { create_channels } from './channels.js';
import { create_map } from './channels_map.js'
import { add_dataset, create_line_live_chart, update } from './charts.js';

$(document).ready(function(){
    let socket = io.connect('http://' + document.domain + ':' + location.port);
    let names = [];
    let ex_chart = create_line_live_chart("execution_time_chart", "Execution Time");
    let drop_chart = create_line_live_chart("drop_rate_chart", "Drop Rate");
    socket.on('update_channels', function(channels){
        create_channels(channels);
        var execution_rates = get_field(channels, "EXECUTION_TIME");
        var drop_rates = get_field(channels, "DROP_RATE")
        if (!( names.length == get_channel_names(channels).length)) {
            names = get_channel_names(channels);
            for (const channel_name of names) {
                add_dataset(ex_chart, channel_name);
                add_dataset(drop_chart, channel_name);
            }
        }
        
        update(ex_chart, execution_rates);
        update(drop_chart, drop_rates);
    });
    socket.on('channels_map', function(dict){
      create_map(dict);
   });
});

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
