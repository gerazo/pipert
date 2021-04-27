import { get_channel_color } from './utils.js';

export function create_map(dict) {
    var nodes = create_nodes(dict.unique_channels);
    var edges = create_edges(dict.channels_map);
    var container = document.getElementById("channels-map");
    var data = {
        nodes: nodes,
        edges: edges
    };
    var options = {};
    new vis.Network(container, data, options);
}

function create_nodes(arr) {
    let ret = [];
    arr.forEach(function(item, index) {
        ret.push({id: index, value:15, shape: "box",
                  color: get_channel_color(item), label: item});
    })

    return ret;
}

function create_edges(dict) {
    let ret = [];
    for (const [a, b] of dict) {
        ret.push({from: a, to: b, arrows: "to"});
    }

    return ret;
}