
// ============================================================================
// web channel init
// ============================================================================

var handler;
var db;

// async channel creation
channel = new QWebChannel(qt.webChannelTransport, function(channel) {
    console.log("QWebChannel created");
    console.log("Available objects:", channel.objects);

    handler = channel.objects.app;
    db = channel.objects.db;

    // setting callbacks here
    windowControls();
    favControls();
    workspaceControls();
    newTabCallback();
    adressBarCallback();
});


// ============================================================================
// tab gui logic
// ============================================================================

tabList = [];

// updating title from qt
function updateTabTitle(index, title) {
    tabList[index].children[1].innerHTML = title;
}

// updating icon from qt
function updateTabIcon(index, icon) {
    if(icon)
        tabList[index].children[0].src = "data:image/png;base64," + icon;
}

// updating url from qt
const addressInputElement = document.getElementById('address-input');

function updateTabURL(url) {
    if(url.startsWith("qrc:"))
        addressInputElement.value = '';
    else
        addressInputElement.value = url;
}

// next tab
function nextTab() {
    let currentTab = tabList.findIndex(function(el) {
        return el.classList.contains('selected');
    });

    tabList[currentTab].classList.remove('selected');
    
    if(currentTab < tabList.length - 1) {
        tabList[currentTab + 1].classList.add('selected');
    } else {
        tabList[0].classList.add('selected');
    }
}

// previous tab
function prevTab() {
    let currentTab = tabList.findIndex(function(el) {
        return el.classList.contains('selected');
    });

    tabList[currentTab].classList.remove('selected');
    
    if(currentTab > 0) {
        tabList[currentTab - 1].classList.add('selected');
    } else {
        tabList[tabList.length - 1].classList.add('selected');
    }
}

// ============================================================================
// address input change
// ============================================================================

const addressFormElement = document.getElementById('adress-form');

function adressBarCallback() {
    addressFormElement.addEventListener('submit', function(event) {
        handler.pageChangeUrl(addressInputElement.value);

        addressInputElement.blur();
        event.preventDefault();
    });
}

// ============================================================================
// new tab
// ============================================================================

const tabListElement = document.getElementById('tab-list');
const newTabElement = document.getElementById('newTabButton');

function newTab(inside=true) {
    // qt creates new tab
    if(inside)
        handler.createTab('');

    // creating tab element inside gui
    tab = document.createElement('div');

    tab.classList.add('tab');
    tab.classList.add('d-flex');
    tab.classList.add('align-items-center');
    tab.classList.add('pt-1');
    tab.classList.add('pb-2');
    tab.classList.add('px-2');

    icon = document.createElement('img');
    icon.src = 'img/globe_small.png';
    icon.classList.add('me-1');

    text = document.createElement('div');
    text.classList.add('pt-1');
    text.innerHTML = 'tab';

    tab.appendChild(icon);
    tab.appendChild(text);

    // adding to tab list
    tabListElement.appendChild(tab);

    let index = tabList.length;
    tabList.push(tab);

    // tab element click
    tab.addEventListener('click', function() {
        let i = tabList.indexOf(this);

        tabList.forEach(function(el) {
            el.classList.remove('selected'); 
        });
        tabList[i].classList.add('selected'); 

        // handling swithing in qt
        handler.changeTab(i);

        // check if bookmark after switching to it
        handler.checkBookmark();
    });

    // tab element closing
    tab.addEventListener('auxclick', function() {
        if(tabList.length == 1) {
            return;
        }

        tab = tabList[index];
        tabList.splice(index, 1);

        tab.remove();

        newIndex = index ? index - 1 : 0;
        tabList[newIndex].classList.add('selected'); 

        // removing in qt
        handler.closeTab(index, newIndex);
    });

    // switching to created tab
    tab.click();

    // update tabs width
    // TODO
    // show scroll buttons on overflow
    // TODO
}


function newTabCallback() {
    document.getElementById('newTabButton').addEventListener('click', function() {
        newTab();
    });
}

function closeCurrentTab() {
    let tab = document.getElementsByClassName('selected')[0];
    tab.dispatchEvent(new Event("auxclick"));
}

// ============================================================================
// fav logic
// ============================================================================

const favRemoveButton = document.getElementById('fav-remove-button');
const modal = new bootstrap.Modal(document.getElementById('favModal'));
const workspaceModal = new bootstrap.Modal(document.getElementById('workspaceModal'));
const addWorkspaceModal = new bootstrap.Modal(document.getElementById('addWorkspaceModal'));
const favButton = document.getElementById('favButton');

function gotFocus() {
    // check if modal is open
    // alert('got focus');
    if(modal._element.classList.contains('show') 
    || workspaceModal._element.classList.contains('show')
    || addWorkspaceModal._element.classList.contains('show')) {
        modal.hide();
        workspaceModal.hide();
        addWorkspaceModal.hide();
    } else {
        addressInputElement.focus();
    }
}

// set fav icon for current page
function setFavIcon(state) {
    if(state) {
        favButton.children[0].classList.remove('shaded-img');
        // favButton.children[0].filter = 'none';
        favButton.children[0].src = 'img/faved.png';
    } else {
        favButton.children[0].classList.add('shaded-img');
        // favButton.children[0].filter = 'brightness(0.6)contrast(3.4)hue-rotate(180deg)saturate(5)';
        favButton.children[0].src = 'img/fav.png';
    }
}

// opening fav dialog
function favDialog() {
    modal.show();
    // modal.focus();
    setFavIcon(true);
}

// fav modal init
const favModalElement = document.getElementById('favModal');
const iconElement = document.getElementById('favIcon');
const textElement = document.getElementById('fav-name');

favModalElement.addEventListener('show.bs.modal', function(event) {
    let tabElement = document.getElementsByClassName('selected')[0];

    iconElement.src = tabElement.children[0].src;
    textElement.value = tabElement.children[1].innerHTML;
});


// fav callbacks
function favControls() {
    // removing from inside fav dialog
    favRemoveButton.addEventListener('click', function() {
        let tabElement = document.getElementsByClassName('selected')[0];
        handler.removeBookmark();
        handler.checkBookmark();

        modal.hide();
    });

    // adding to favs by click
    favButton.addEventListener('click', function() {
        handler.favClick();
        handler.checkBookmark();
    });
}

// ============================================================================
// workspace logic
// ============================================================================

const workspaceListElement = document.getElementById('workspace-list');
const addWorkspaceButton = document.getElementById('workspace-create-button');
const workspaceBtn = document.getElementById('workspaceButton');

let currentWorkspace;

// setting current workspace color
function setWorkspaceColor(color) {
    workspaceBtn.style.setProperty('background-color', color, 'important');
}

function setCurrentWorkspace(workspace) {
    currentWorkspace = workspace;
}

// workspace callbacks
function workspaceControls() {
    // workspace selection button
    document.getElementById('workspaceButton').addEventListener('click', function() {
        db.getWorkspaces();
    });

    // getting list from qt
    db.workspacesReady.connect(function(result) {
        // document.getElementById("output").innerText = result;  // Display result.
        let workspaces = JSON.parse(result);

        workspaceListElement.innerHTML = '';

        for(let i = 0; i < workspaces.length; i++) {
            let workspace = workspaces[i];

            let workspaceElement = document.createElement('div');
            workspaceElement.classList.add('workspace-item');
            workspaceElement.classList.add('d-flex');
            workspaceElement.classList.add('align-items-center');
            workspaceElement.classList.add('gap-1');
            workspaceElement.classList.add('pt-1');
            workspaceElement.classList.add('pb-1');
            workspaceElement.classList.add('px-2');

            if(workspace.id == currentWorkspace) {
                workspaceElement.classList.add('selected-workspace');
            }

            let color = document.createElement('div');
            color.style.backgroundColor = workspace.color;
            color.style.width = '24px';
            color.style.height = '24px';
            color.style.minWidth = '24px';

            let textContainer = document.createElement('div');
            textContainer.classList.add('d-flex');
            textContainer.classList.add('flex-column');
            textContainer.classList.add('justify-content-center');

            let name = document.createElement('div');
            name.innerHTML = workspace.name;

            let description = document.createElement('div');
            description.classList.add('text-secondary');
            description.innerHTML = workspace.description;

            textContainer.appendChild(name);
            textContainer.appendChild(description);
            workspaceElement.appendChild(color);
            workspaceElement.appendChild(textContainer);
            workspaceListElement.appendChild(workspaceElement);

            // workspaceElement.setAttribute('data-id', workspace.id);
            workspaceElement.addEventListener('click', function() {
                setWorkspaceColor(workspace.color);

                db.selectWorkspace(workspace.id);
                // closing all tabs
                handler.closeAllTabs();
                tabListElement.innerHTML = '';
                tabList = [];
                // loading tabs from new workspace
                handler.loadWorkspaceTabs(workspace.id);

                workspaceModal.hide();
                currentWorkspace = workspace.id;
            })
        }
    });

    // creating new workspace
    addWorkspaceButton.addEventListener('click', function() {
        let name = document.getElementById('workspace-name');
        let description = document.getElementById('workspace-description');
        let color = document.getElementById('workspace-color');

        if(!name.value) {
            name.focus();
            return;
        }

        if(!description.value) {
            description.focus();
            return;
        }

        db.addWorkspace(name.value, color.value, description.value)
        .then(function(result) {
            currentWorkspace = result;
        });

        setWorkspaceColor(color.value);

        // reset
        addWorkspaceModal.hide();
        name.value = '';
        description.value = '';
        color.value = '#808080';
    });
}

const container = document.getElementById('workspace-list');
container.addEventListener('wheel', (event) => {
    event.preventDefault();
    container.scrollLeft += event.deltaY;
});

// ============================================================================
// window and page controls
// ============================================================================

// window callbacks
function windowControls() {
    // top close
    document.getElementById('closeButton').addEventListener('click', function() {
        handler.closeWindow();
    });

    // top maximize
    document.getElementById('maximizeButton').addEventListener('click', function() {
        handler.toggleMaximize();
    });

    // top minimize
    document.getElementById('minimizeButton').addEventListener('click', function() {
        handler.minimizeWindow();
    });

    // page back
    document.getElementById('backButton').addEventListener('click', function() {
        handler.pageBack();
    });

    // page forward
    document.getElementById('forwardButton').addEventListener('click', function() {
        handler.pageForward();
    });

    // page refresh
    document.getElementById('refreshButton').addEventListener('click', function() {
        handler.pageReload();
    });

    // drag window (passing event to qt)
    const dragElement = document.getElementById('space');

    dragElement.addEventListener('mousedown', function() {
        handler.startMove();
    });
}

console.log('controls script loaded');