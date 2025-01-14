
// ============================================================================
// markdown logic
// ============================================================================



// ============================================================================
// qt webchannel logic
// ============================================================================

var handler;
var db;

// data arrays
let nodes_input = [];
let links_input = [];

channel = new QWebChannel(qt.webChannelTransport, function(channel) {
    console.log("QWebChannel created for graph");
    console.log("Available objects:", channel.objects);

    handler = channel.objects.app;
    db = channel.objects.db;

    // setting callbacks here
    Promise.all([
        handler.getNodes(),            
        handler.getLinks()
    ])
    .then(function(results) {
        nodes_input = JSON.parse(results[0]);
        links_input = JSON.parse(results[1]);

        console.log(nodes_input);
        console.log(links_input);

        MathJax.startup.promise.then(() => {
            // MathJax is ready, you can now use its functions
            console.log('MathJax is ready');
        }).catch((err) => console.log('MathJax failed to load: ' + err.message));

        drawGraph();
    })
});

// ============================================================================
// graph drawing logic
// ============================================================================

// INPUT FORMAT
// notes: [], links: [], note_groups: []
// note { id, workspace_id, title, content, group_id }
// link { source_id, target_id }
// note_group { id, color }

// TODO node colors
// TODO link additional properties

function clamp(x, lo, hi) {
  return x < lo ? lo : x > hi ? hi : x;
}

function drawGraph() {
    let container = document.getElementById("graph-container");
    let contentElement = document.getElementById("note-content");

    let width = container.offsetWidth;
    let height = container.offsetHeight;
    // reset canvas
    container.innerHTML = "";

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
      .force("link", d3.forceLink(links).id(d => d.id))
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
      // .attr("stroke", "#fff")
      // .attr("stroke-width", 1.5)
      .attr("class", "node")
    .selectAll("g")
    .data(nodes)
    .join("g");

    // Add a circle for each node.
    node.append("circle")
        .attr("r", 6)
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
    function dragged(event, d) {
        // event.subject.fx = event.x;
        // event.subject.fy = event.y;

        d.fx = clamp(event.x, 0, width);
        d.fy = clamp(event.y, 0, height);
        simulation.alpha(1).restart();
    }

    // Restore the target alpha so the simulation cools after dragging ends.
    // Unfix the subject position now that it’s no longer being dragged.
    function dragended(event) {
        if (!event.active) simulation.alphaTarget(0);
        // event.subject.fx = null;
        // event.subject.fy = null;
    }

    // Selecting node
    node.on("click", click);

    function click(event, d) {
        // init markdownit
        const md = window.markdownit({
            breaks: true // Enable line breaks
        });

        // rendering note
        contentElement.innerHTML = md.render(d.content);
        hljs.highlightAll();
        MathJax.typesetPromise([contentElement])
            .catch((err) => console.log('Typeset failed: ' + err.message));


        // deselect all
        d3.selectAll(".selected").classed("selected", false);

        // add class to node circle and text
        d3.select(this).select("circle").classed("selected", true);
        d3.select(this).select("text").classed("selected", true);

        // unfixing position
        simulation.alpha(1).restart();
        // delete d.fx;
        // delete d.fy;
    }

    // When this cell is re-run, stop the previous simulation. (This doesn’t
    // really matter since the target alpha is zero and the simulation will
    // stop naturally, but it’s a good practice.)
    // invalidation.then(() => simulation.stop());
}

// updatin on windodw resize
window.addEventListener('resize', function() {
    width = container.offsetWidth;
    height = container.offsetHeight;

    drawGraph();
});

// ============================================================================
// modal logic
// ============================================================================

const modal = new bootstrap.Modal(document.getElementById('addModal'));
const addNoteBtn = document.getElementById('add-note-button');

const title = document.getElementById('new-title');
const content = document.getElementById('new-content');

addNoteBtn.addEventListener('click', function() {
    // check if title is not empty
    if(title.value == "") {
        title.focus();
        return;
    }

    // adding new note
    // TODO add group
    handler.addNote(title.value, content.value);
});