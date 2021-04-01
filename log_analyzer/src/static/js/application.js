$(document).ready(function(){
    let socket = io.connect('http://' + document.domain + ':' + location.port);
    let drop_rate_chart = draw_chart([], []);
    
    socket.on('update_channels', function(channels){
        $(".channels-container").empty();
        $.each(channels, function(i, channel){
            const channel_div = create_channel(channel);
            $(".channels-container").append(channel_div);
        });
        channel_names = get_channel_names(channels);
        drop_rates = get_drop_rates(channels);
        update_chart(drop_rate_chart, channel_names, drop_rates);
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


function draw_chart(channel_names, drop_rates){
    let myChart = document.getElementById('myChart').getContext('2d');

    // Chart.defaults.global.defaultFontFamily = 'Lato';
    // Chart.defaults.global.defaultFontSize = 18;
    // Chart.defaults.global.defaultFontColor = '#777';

    let drop_rate_chart = new Chart(myChart, {
        type:'horizontalBar', // bar, horizontalBar, pie, line, doughnut, 
                             // radar, polarArea
        data:{
          labels:channel_names,
          datasets:[{
            label:'Drop Rates',
            data: drop_rates,
            backgroundColor:'green',
            backgroundColor:[
              'rgba(255, 99, 132, 0.6)',
              'rgba(54, 162, 235, 0.6)',
              'rgba(255, 206, 86, 0.6)',
              'rgba(75, 192, 192, 0.6)',
              'rgba(153, 102, 255, 0.6)',
              'rgba(255, 159, 64, 0.6)',
              'rgba(255, 99, 132, 0.6)'
            ],
            borderWidth:1,
            borderColor:'#777',
            hoverBorderWidth:3,
            hoverBorderColor:'#000'
          }]
        },
        options:{
          title:{
            display:true,
            text:'Drop Rates',
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

    return drop_rate_chart;
}

function get_channel_names(channels){
    names = []
    channels.forEach(function(channel){
        names.push(channel.name);
    });
    
    return names
}

function get_drop_rates(channels){
    rates = []
    channels.forEach(function(channel){
        rates.push(channel.DROP_RATE);
    });
    
    return rates
}

function update_chart(chart, labels, data) {
    chart.data.datasets[0].data = data;
    chart.data.labels = labels;
    chart.update();
}
