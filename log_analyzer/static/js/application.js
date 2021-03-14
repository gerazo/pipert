let channels = [];

// function does_exist(channel_name){
//     const channel = channels.find(channel => channel.name === channel_name)
//     if (channel === undefined){
//         return false
//     }

//     return true
// }

// function parse_channel(packet){
//     if (!(does_exist(packet.receiver_channel))){
//         var channel_1 = {
//             "name": packet.receiver_channel,
//             "events": packet.events
//         };

//         channels.push(channel_1);
//     }

//    if (!(does_exist(packet.sender_channel))){
//         var channel_1 = {
//             "name": packet.sender_channel,
//             "events": []
//         };

//         channels.push(channel_1);
//     }

// }

$(document).ready(function(){

    var socket = io.connect('http://' + document.domain + ':' + location.port);

//    socket.on('connect', function() {
//        socket.send("Connection established")
//    });
//
//    socket.emit('event', 'Woahhh')
//
//    socket.on('message', function(msg){
//        alert(msg)
//    })

    socket.on('update_channels', function(channels){
        // parse_channel(packet);
        $(".channels-container").empty();
        $.each(channels, function(i, channel){
            $(".channels-container").append("<h1>"+channel.name+"</h1>");
            $(".channels-container").append("<h2>"+channel.flags.FROZEN+"</h2>");
        });
//        console.log(packet);
//        $("body").append("<h1>Packet:-<h1>");
//        $("body").append("<h2>"+packet.receiver_channel+"<h2>");
//        $("body").append("<h2>"+packet.sender_channel+"<h2>");
//        $.each(packet.events, function(i, event){
//            $("body").append("<h3>"+event.type   +"<h3>");
//        });
    });
});