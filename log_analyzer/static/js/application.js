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

    socket.on('packet_name', function(packet){
        alert(packet.name)
    });
});