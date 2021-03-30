$(document).ready(function(){
    var socket = io.connect('http://' + document.domain + ':' + location.port);

    socket.on('update_channels', function(channels){
        $(".channels-container").empty();
        $.each(channels, function(i, channel){
            const channel_div = create_channel(channel);
            $(".channels-container").append(channel_div);
        });
    });
});

function create_channel(channel){
    const channel_div = document.createElement("div");
    const name = create_element_with_text("H2", channel.name);
    channel_div.appendChild(name);
    create_flags(channel, channel_div);
    channel_div.className = "channel";

    return channel_div
}

function create_element_with_text(elem, text){
    const e = document.createElement(elem);
    const t = document.createTextNode(text);
    e.appendChild(t);
    return e
}

function create_flags(channel, channel_div){
    for (const [key, value] of Object.entries(channel.flags)) {
        create_flag(channel_div, key, value);
    }
}

function create_flag(channel_div, key, val){
    let text = ''
    if (val){
        text = `${key}` + "       ⛔";
    }else {
        text = `${key}` + "       ✅";
    }
    const flag = create_element_with_text("P", text);
    channel_div.appendChild(flag);
}


$(document).ready(function(){
    let myChart = document.getElementById('myChart').getContext('2d');

    Chart.defaults.global.defaultFontFamily = 'Lato';
    Chart.defaults.global.defaultFontSize = 18;
    Chart.defaults.global.defaultFontColor = '#777';

    let massPopChart = new Chart(myChart, {
        type:'bar', // bar, horizontalBar, pie, line, doughnut, radar, polarArea
        data:{
          labels:['Boston', 'Worcester', 'Springfield', 'Lowell', 'Cambridge'],
          datasets:[{
            label:'Population',
            data:[
              617594,
              181045,
              153060,
              106519,
              105162
            ],
            //backgroundColor:'green',
            backgroundColor:[
              'rgba(255, 99, 132, 0.6)',
              'rgba(54, 162, 235, 0.6)',
              'rgba(255, 206, 86, 0.6)',
              'rgba(75, 192, 192, 0.6)',
              'rgba(153, 102, 255, 0.6)',
              'rgba(255, 159, 64, 0.6)',
              'rgba(255, 99, 132, 0.6)'
            ]
          }]
        }
      });
});