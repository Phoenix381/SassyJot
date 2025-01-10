

let container = document.getElementById("graph-container");
// Declare the chart dimensions and margins.
const width = container.offsetWidth;
const height = container.offsetHeight;

// not sure if needed
// const marginTop = 20;
// const marginRight = 20;
// const marginBottom = 20;
// const marginLeft = 20;

// INPUT FORMAT
// notes: [], links: [], note_groups: []
// note { id, workspace_id, title, content, group_id }
// link { source, target }
// note_group { id, color }

// TODO node colors
// TODO link additional properties


// TEST DATA
let nodes_input = [
    {
      id: 1,
      title: "a",
      group_id: 1
    },
    {
      id: 2,
      title: "b",
      group_id: 2
    },
    {
      id: 3,
      title: "c",
      group_id: 2
    },
    {
      id: 4,
      title: "d",
      group_id: 4
    },
    {
      id: 5,
      title: "e",
      group_id: 5
    }
]
let links_input = [
    {   
        source: "a",   
        target: "b",
        value: 1
    },
    {
        source: "a",   
        target: "c",
        value: 2
    },
    {
        source: "c",      
        target: "d",
        value: 3
    },
    {
        source: "b",      
        target: "e",
        value: 4
    },
    {
        source: "b",         
        target: "c",
        value: 5
    }
]


// TODO color selection
// Specify the color scale.
const color = d3.scaleOrdinal(d3.schemeCategory10);
const textColor = window.getComputedStyle(document.body).getPropertyValue('--text-color');

// The force simulation mutates links and nodes, so create a copy
// so that re-evaluating this cell produces the same result.
const links = links_input.map(d => ({...d}));
const nodes = nodes_input.map(d => ({...d}));

// Create a simulation with several forces.
const simulation = d3.forceSimulation(nodes)
  .force("link", d3.forceLink(links).id(d => d.title))
  .force("charge", d3.forceManyBody())
  .force("center", d3.forceCenter(width / 2, height / 2))
  .on("tick", ticked);

// Create an SVG element.
const svg = d3.select("#graph-container")
  .append("svg")
  .attr("width", width)
  .attr("height", height);

// Add a line for each link, and a circle for each node.
const link = svg.append("g")
  .attr("stroke", "#999")
  .attr("stroke-opacity", 0.6)
.selectAll()
.data(links)
.join("line")
  // .attr("stroke-width", d => Math.sqrt(d.value));

// Create node groups.
const node = svg.append("g")
  .attr("stroke", "#fff")
  .attr("stroke-width", 1.5)
  .attr("class", "node")
.selectAll("g")
.data(nodes)
.join("g");

// Add a circle for each node.
node.append("circle")
    .attr("r", 5)
    .attr("fill", d => color(d.group_id));

// Add id labels for each node.
node.append("text")
    .attr("fill", textColor)
    .attr("dy", 24)
    .attr("text-anchor", "middle")
    .text(d => d.title);

// Add drag behaviors to each node.
node.call(d3.drag()
    .on("start", dragstarted)
    .on("drag", dragged)
    .on("end", dragended));

// Set the position attributes of links and nodes each time the simulation ticks.
function ticked() {
link
    .attr("x1", d => d.source.x)
    .attr("y1", d => d.source.y)
    .attr("x2", d => d.target.x)
    .attr("y2", d => d.target.y);

node
    .attr("transform", d => `translate(${d.x},${d.y})`);
}

// Reheat the simulation when drag starts, and fix the subject position.
function dragstarted(event) {
    if (!event.active) simulation.alphaTarget(0.3).restart();
    event.subject.fx = event.subject.x;
    event.subject.fy = event.subject.y;
}

// Update the subject (dragged node) position during drag.
function dragged(event) {
    event.subject.fx = event.x;
    event.subject.fy = event.y;
}

// Restore the target alpha so the simulation cools after dragging ends.
// Unfix the subject position now that it’s no longer being dragged.
function dragended(event) {
    if (!event.active) simulation.alphaTarget(0);
    event.subject.fx = null;
    event.subject.fy = null;
}

// When this cell is re-run, stop the previous simulation. (This doesn’t
// really matter since the target alpha is zero and the simulation will
// stop naturally, but it’s a good practice.)
// invalidation.then(() => simulation.stop());
