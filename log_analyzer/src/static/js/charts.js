import { get_channel_color } from './utils.js';

export function create_line_live_chart(id, title) {
    var ctx = document.getElementById(id).getContext('2d');
    var config = {
        type: 'line',
        labels: [pretty_date(Date.now())],
        data: [],
        options: {
            title: {
                display: true,
                text: title,
                fontSize: 25
            }
        }
    }
    
    return new Chart(ctx, config);
}

export function add_dataset(chart, channel) {
    var dataset = {
            label: channel,
            borderColor: get_channel_color(channel),
            backgroundColor: get_channel_color(channel),
            fill: false
    }
    chart.data.datasets.push(dataset);
    chart.update();
}

export function update(chart, data) {
    chart.data.labels.push(pretty_date(Date.now()));
    if (chart.data.labels.length > 100) {
        chart.data.labels.shift();
    }
    let i = 0;
    chart.data.datasets.forEach((dataset) => {
        dataset.data.push(data[i]);
        if (dataset.data.length > 100) {
            dataset.data.shift();
        }
        i++;
    });

    chart.update();
}

function pretty_date(time){
    var date = new Date(parseInt(time));

    return date.toLocaleTimeString();;
}
