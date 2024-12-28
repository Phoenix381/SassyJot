// element selection actions
addressInput = document.getElementById('address-input');
addressBar = document.getElementById('address-bar');

newTabElement = document.getElementById('newTabButton');
addressInputElement = document.getElementById('address-input');
addressFormElement = document.getElementById('adress-form');

// addressInput.addEventListener('focus', function() {
//     addressBar.style.border = '1px solid #00a0ff';
// });
// addressInput.addEventListener('blur', function() {
//     addressBar.style.border = '1px solid #aaa';
// });

// tab
tabListElement = document.getElementById('tab-list');
tabList = [];

function updateTabTitle(index, title) {
    tabList[index].children[1].innerHTML = title;
}

function updateTabIcon(index, icon) {
    if(icon)
        tabList[index].children[0].src = "data:image/png;base64," + icon;
}

function updateTabURL(url) {
    if(url.startsWith("qrc:"))
        addressInputElement.value = '';
    else
        addressInputElement.value = url;
}

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


let favRemoveButton = document.getElementById('fav-remove-button');
let modal = new bootstrap.Modal(document.getElementById('favModal'));
let favButton = document.getElementById('favButton');

function gotFocus() {
    // check if modal is open
    // alert('got focus');
    if(modal._element.classList.contains('show')) {
        modal.hide();
    } else {
        addressInputElement.focus();
    }
}

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

function favDialog() {
    modal.show();
    // modal.focus();
    setFavIcon(true);
}

addressFormElement.addEventListener('submit', function(event) {
    event.preventDefault();
    new QWebChannel(qt.webChannelTransport, function(channel) {
        var handler = channel.objects.clickHandler;
        handler.requestUrlChange(addressInputElement.value);

        addressInputElement.blur();
    });
});


function newTab() {
    new QWebChannel(qt.webChannelTransport, function(channel) {
        var handler = channel.objects.clickHandler;

        handler.requestNewTab();

        tab = document.createElement('div');

        tab.classList.add('tab');
        tab.classList.add('d-flex');
        tab.classList.add('align-items-center');
        tab.classList.add('pt-1');
        tab.classList.add('pb-2');
        tab.classList.add('px-2');

        icon = document.createElement('img');
        icon.src = 'img/globe_small.png';
        // icon.width = 16;
        icon.classList.add('me-1');
        // icon.style.transform = 'scale(1.4)';

        text = document.createElement('div');
        text.classList.add('pt-1');
        text.innerHTML = 'tab';

        tab.appendChild(icon);
        tab.appendChild(text);

        // adding to tab list
        tabListElement.appendChild(tab);

        let index = tabList.length;
        tabList.push(tab);

        tab.addEventListener('click', function() {
            tabList.forEach(function(el) {
                el.classList.remove('selected'); 
            });
            tabList[index].classList.add('selected'); 

            handler.requestChangeTab(index);

            // console.log(index);

            // check if faved
            handler.checkBookmark();
        });

        tab.addEventListener('auxclick', function() {
            if(tabList.length == 1) {
                return;
            }

            tab = tabList[index];
            tabList.splice(index, 1);

            tab.remove();

            newIndex = index ? index - 1 : 0;
            tabList[newIndex].classList.add('selected'); 

            handler.requestCloseTab(index, newIndex);
        });

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

// qt webchannel
new QWebChannel(qt.webChannelTransport, function(channel) {
    var handler = channel.objects.clickHandler;

    document.getElementById('closeButton').addEventListener('click', function() {
        handler.requestClose();
    });

    document.getElementById('maximizeButton').addEventListener('click', function() {
        handler.requestMaximize();
    });

    document.getElementById('minimizeButton').addEventListener('click', function() {
        handler.requestMinimize();
    });

    document.getElementById('backButton').addEventListener('click', function() {
        handler.requestBack();
    });

    document.getElementById('forwardButton').addEventListener('click', function() {
        handler.requestForward();
    });

    document.getElementById('refreshButton').addEventListener('click', function() {
        handler.requestReload();
    });

    favRemoveButton.addEventListener('click', function() {
        let tabElement = document.getElementsByClassName('selected')[0];
        // let url = tabElement.children[1].innerHTML;
        handler.removeBookmark();
        handler.checkBookmark();

        modal.hide();
    });

    // drag window
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