import { get_channel_color } from './utils.js';

/*
* Creating a map of the channels in the pipeline using the vis.js library
* */

export function create_map(dict) {
    const nodes = create_nodes(dict.unique_channels);
    const edges = create_edges(dict.channels_map);
    const container = document.getElementById("channels-map");
    const data = {
        nodes: nodes,
        edges: edges
    };
    const options = {};
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