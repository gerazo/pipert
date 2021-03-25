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
    const flag = create_element_with_text("P1", `${key}: ${val}`);
    channel_div.appendChild(flag);
}
