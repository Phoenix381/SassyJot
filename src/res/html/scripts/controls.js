// element selection actions
// addressInput = document.getElementById('address-input');
addressBar = document.getElementById('address-bar');
addressFormElement = document.getElementById('adress-form');
addressInputElement = document.getElementById('address-input');

newTabElement = document.getElementById('newTabButton');

console.log('controls loaded');

// ============================================================================
// tab logic
// ============================================================================

tabListElement = document.getElementById('tab-list');
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
// fav logic
// ============================================================================

let favRemoveButton = document.getElementById('fav-remove-button');
let modal = new bootstrap.Modal(document.getElementById('favModal'));
let workspaceModal = new bootstrap.Modal(document.getElementById('workspaceModal'));
let favButton = document.getElementById('favButton');

function gotFocus() {
    // check if modal is open
    // alert('got focus');
    if(modal._element.classList.contains('show') || workspaceModal._element.classList.contains('show')) {
        modal.hide();
        workspaceModal.hide();
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

// ============================================================================
// address input change
// ============================================================================

addressFormElement.addEventListener('submit', function(event) {
    event.preventDefault();
    new QWebChannel(qt.webChannelTransport, function(channel) {
        var handler = channel.objects.clickHandler;
        handler.requestUrlChange(addressInputElement.value);

        addressInputElement.blur();
    });
});

// ============================================================================
// new tab
// ============================================================================

function newTab() {
    new QWebChannel(qt.webChannelTransport, function(channel) {
        var handler = channel.objects.clickHandler;

        // qt creates new tab
        handler.requestNewTab();

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
            tabList.forEach(function(el) {
                el.classList.remove('selected'); 
            });
            tabList[index].classList.add('selected'); 

            // handling swithing in qt
            handler.requestChangeTab(index);

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
            handler.requestCloseTab(index, newIndex);
        });

        // switching to created tab
        tab.click();
    });

    // update tabs width
    // TODO
    // show scroll buttons on overflow
    // TODO
}

document.getElementById('newTabButton').addEventListener('click', function() {
    newTab();
});

function closeCurrentTab() {
    let tab = document.getElementsByClassName('selected')[0];
    tab.dispatchEvent(new Event("auxclick"));
}

// ============================================================================
// button clicking
// ============================================================================

new QWebChannel(qt.webChannelTransport, function(channel) {
    var handler = channel.objects.clickHandler;

    // top close
    document.getElementById('closeButton').addEventListener('click', function() {
        handler.requestClose();
    });

    // top maximize
    document.getElementById('maximizeButton').addEventListener('click', function() {
        handler.requestMaximize();
    });

    // top minimize
    document.getElementById('minimizeButton').addEventListener('click', function() {
        handler.requestMinimize();
    });

    // page back
    document.getElementById('backButton').addEventListener('click', function() {
        handler.requestBack();
    });

    // page forward
    document.getElementById('forwardButton').addEventListener('click', function() {
        handler.requestForward();
    });

    // page refresh
    document.getElementById('refreshButton').addEventListener('click', function() {
        handler.requestReload();
    });

    // removing from inside fav dialog
    favRemoveButton.addEventListener('click', function() {
        let tabElement = document.getElementsByClassName('selected')[0];
        handler.removeBookmark();
        handler.checkBookmark();

        modal.hide();
    });

    // drag window (papssing event)
    const dragElement = document.getElementById('space');

    dragElement.addEventListener('mousedown', function() {
        handler.startMove();
    });
});

// fav modal
let favModalElement = document.getElementById('favModal');
let iconElement = document.getElementById('favIcon');
let textElement = document.getElementById('fav-name');

favModalElement.addEventListener('show.bs.modal', function(event) {
    let tabElement = document.getElementsByClassName('selected')[0];

    iconElement.src = tabElement.children[0].src;
    textElement.value = tabElement.children[1].innerHTML;
});