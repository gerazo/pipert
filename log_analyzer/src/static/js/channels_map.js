import { get_channel_color } from './utils.js';

export function create_map(channels) {
    var nodes = create_nodes(channels);
    var edges = create_edges(channels);
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
    arr.forEach(function(item, index, array) {
        ret.push({id: index, value:12, shape: "box",
                  color: get_channel_color(item), label: item});
    })

    return ret;
}

function create_edges(arr) {
    let ret = [];
    for (let i = 0; i < (arr.length - 1); i++) {
        ret.push({from: i, to: i+1, arrows: "to"});
    }

    return ret;
}