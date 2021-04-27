import { get_channel_color } from './utils.js';

export function create_channels(channels) {
    const container = document.getElementsByClassName("channels-container")[0];
    container.innerHTML = "";
    for (const channel of channels) {
        const channel_div = create_channel(channel);
        container.appendChild(channel_div);
    }
}

function create_channel(channel){
    const channel_div = document.createElement("div");
    const name = create_element_with_text("H2", channel.name);
    name.style.color = get_channel_color(channel.name); 
    channel_div.appendChild(name);
    create_flags(channel, channel_div);
    channel_div.className = `channel ${channel.name}`;
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