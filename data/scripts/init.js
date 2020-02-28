//load
require("data/scripts/gl-matrix-min.js");
Object.assign(window, glMatrix);
//require("data/scripts/litegl.min.js");
require("data/scripts/litegraph.js");
require("data/scripts/HBTree.js");
require("data/scripts/facade.js");
require("data/scripts/polyfill.js");


var g = new HBTGraph("pepe");
var data = JSON.parse('{"last_node_id":11,"last_link_id":8,"nodes":[{"id":8,"type":"btree/Root","pos":[427,85],"size":[140,26],"flags":{},"order":0,"mode":0,"outputs":[{"name":"","type":"path","links":[7]}],"properties":{},"color":"#1E1E1E","boxcolor":"#999","shape":2},{"id":10,"type":"btree/Succeeder","pos":[437,232],"size":[120,20],"flags":{},"order":1,"mode":0,"inputs":[{"name":"","type":"path","link":7}],"properties":{},"color":"#2e542e","bgcolor":"#496b49","boxcolor":"#999","shape":2}],"links":[[7,8,0,10,0,"path"]],"groups":[],"config":{},"version":0.4}');
g.graph.configure(data);

