'use strict';

if (process.platform !== 'win32') return;

const { hookKeyboard } = require('bindings')('ll_keyboard_hooks');

let hooks = {};

hookKeyboard((data) => {
  const keyCodePattern = data.toString().trim().replace(/\n/g, '');
  const keyCodeEvent = keyCodePattern.split('::')[0];
  const keyCode = keyCodePattern.split('::')[1];
  const eventObj = {
    event: keyCodeEvent,
    vkCode: keyCode,
  };

  if (hooks[keyCodeEvent] && hooks[keyCodeEvent][keyCode]) {
    hooks[keyCodeEvent][keyCode].forEach((fn) => fn(eventObj));
  }
  if (hooks[keyCodeEvent] && hooks[keyCodeEvent]['*']) {
    hooks[keyCodeEvent]['*'].forEach((fn) => fn(eventObj));
  }
  if (hooks['*'] && hooks['*']['*']) {
    hooks['*']['*'].forEach((fn) => fn(eventObj));
  }
});

const KEY_NAMES = {
  'Control': [162, 163],
  'Shift': [160, 161],
  'MediaPlayPause': [179],
  'MediaStop': [178],
  'MediaPreviousTrack': [177],
  'MediaNextTrack': [176]
};

for (let F = 1; F <= 24; F++) {
  KEY_NAMES[`F${F}`] = 112 + F - 1;
}

const _hook = (keyCodeEvent, code, fn) => {
  hooks[keyCodeEvent] = hooks[keyCodeEvent] || {};
  hooks[keyCodeEvent][code] = hooks[keyCodeEvent][code] || [];
  hooks[keyCodeEvent][code].push(fn);
}

module.exports = {
  on: (keyCodeEvent, keyCode, fn) => {
    if (KEY_NAMES[keyCode]) {
      let codes = KEY_NAMES[keyCode];
      codes.forEach((code) => _hook(keyCodeEvent, code, fn));
    } else {
      _hook(keyCodeEvent, keyCode, fn);
    }
  },
  unbind: () => {
    hooks = {};
    return true;
  }
};
