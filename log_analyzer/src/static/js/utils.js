let channels_colors = {}

export function get_channel_color(channel) {
    if (!(channel in channels_colors)) {
       channels_colors[channel] = get_random_color(); 
    }

    return channels_colors[channel];
}

function rand(min, max) {
    return min + Math.random() * (max - min);
}

function get_random_color() {
    var h = rand(0, 360);
    var s = rand(50, 100);
    var l = rand(30, 100);
    return 'hsl(' + h + ',' + s + '%,' + l + '%)';
}
