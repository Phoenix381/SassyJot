
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

// currently selected node
let selected_node = null;

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
      .force("link", d3.forceLink(links).id(d => d.id).distance(100))
      .force("charge", d3.forceManyBody().strength(-300))
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
        // enabling edit button
        document.getElementById("edit-button").disabled = false;

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

        // loading data to edit modal
        old_title.value = d.title;
        old_content.value = d.content;
        selected_node = d.id;
    }

    // When this cell is re-run, stop the previous simulation. (This doesn’t
    // really matter since the target alpha is zero and the simulation will
    // stop naturally, but it’s a good practice.)
    // invalidation.then(() => simulation.stop());
}

// updatin on windodw resize
let container = document.getElementById("graph-container");

window.addEventListener('resize', function() {
    width = container.offsetWidth;
    height = container.offsetHeight;

    drawGraph();
});

// ============================================================================
// modal logic
// ============================================================================

const addModal = new bootstrap.Modal(document.getElementById('addModal'));
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

const editModal = new bootstrap.Modal(document.getElementById('editModal'));
const editNoteBtn = document.getElementById('edit-note-button');

const old_title = document.getElementById('old-title');
const old_content = document.getElementById('old-content');

editNoteBtn.addEventListener('click', function() {
    // check if title is not empty
    if(old_title.value == "") {
        old_title.focus();
        return;
    }

    // updating note
    // TODO update group
    handler.updateNote(selected_node, old_title.value, old_content.value);
});

// ============================================================================
// textarea logic
// ============================================================================

// search notes by title in nodes_input
function searchNotes(text) {
    let results = [];

    for (let i = 0; i < nodes_input.length; i++) {
        if (nodes_input[i].title.toLowerCase().includes(text.toLowerCase())) {   
            results.push(nodes_input[i]);
        }
    }

    return results;
}

// draw results selection element
function drawResults(results, element) {
    // get cursor coordinates
    const selection = window.getSelection();
    const range = selection.getRangeAt(0);
    const rect = range.getBoundingClientRect();

    if (rect) {
        const x = rect.left + rect.width / 2;
        const y = rect.top + rect.height / 2;

        // removing if exists
        let prevElement = document.getElementById('search-selection');
        if (prevElement) {
            prevElement.remove();
        }

        // creating result container
        const resultsContainer = document.createElement('div');
        resultsContainer.classList.add('search-selection');
        resultsContainer.id = 'search-selection';
        resultsContainer.style.position = 'absolute';
        resultsContainer.style.zIndex = 9999;

        // fill it with results
        // const resultsContainer = document.createElement('ul');

        for (let result of results) {
            const resultElement = document.createElement('div');
            resultElement.textContent = result.title;
            resultsContainer.appendChild(resultElement);
        }

        // show results
        resultsContainer.style.top = y + 'px';
        resultsContainer.style.left = x + 'px';
        // resultElement.appendChild(resultsContainer);
        document.body.appendChild(resultsContainer);
    }
}

// textareas input in modals
const textareas = document.getElementsByClassName('textarea');
let searching = false;

for (let i = 0; i < textareas.length; i++) {
    textareas[i].addEventListener('input', function() {
        // detect link start with [[
        // const noteLinkRegex = /\[\[(.*?)\]\]/g;
        // const noteLinkStart = /\[\[/g;

        // starting search
        if (textareas[i].textContent.endsWith('[[')) {
            searching = true;
        }

        // show search result element
        if (searching) {
            // select text after [[ in textarea
            let start = textareas[i].textContent.lastIndexOf('[[') + 2;
            let text = textareas[i].textContent.substring(start);

            const results = searchNotes(text);
            drawResults(results, textareas[i]);
        }

    });

    textareas[i].addEventListener('keydown', function(e) {
        if (event.key === 'Backspace') {
            // removing search result element
            if (textareas[i].textContent.endsWith('[[')) {
                searching = false;
                let prevElement = document.getElementById('search-selection');
                if (prevElement) {
                    prevElement.remove();
                }
            }

            // check if removing character of link


            console.log('Backspace pressed');
            // event.preventDefault(); // Prevent the default backspace behavior if needed
        }
    });

    textareas[i].addEventListener('paste', function(e) {
        event.preventDefault();

        // check if pasting link
        const pastedContent = (event.clipboardData || window.clipboardData).getData('text');
        const linkRegex = /https?:\/\/\S+/gi;

        if (linkRegex.test(pastedContent)) {
            // insert as <a>
            document.execCommand('insertHTML', false, `<a href="${pastedContent}">${pastedContent}</a>`);
        }
        else
            document.execCommand('insertText', false, pastedContent);

        // TODO process images etc
    });
}