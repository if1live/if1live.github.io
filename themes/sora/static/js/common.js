// Avoid `console` errors in browsers that lack a console.
(function() {
  var method;
  var noop = function () {};
  var methods = [
    'assert', 'clear', 'count', 'debug', 'dir', 'dirxml', 'error',
    'exception', 'group', 'groupCollapsed', 'groupEnd', 'info', 'log',
    'markTimeline', 'profile', 'profileEnd', 'table', 'time', 'timeEnd',
    'timeStamp', 'trace', 'warn'
  ];
  var length = methods.length;
  var console = (window.console = window.console || {});

  while (length--) {
    method = methods[length];

    // Only stub undefined methods.
    if (!console[method]) {
      console[method] = noop;
    }
  }
}());

// Place any jQuery/helper plugins in here.

var tables = document.querySelectorAll('.article-content table');
for(var i = 0 ; i < tables.length ; i++) {
  tables[i].classList.add('pure-table');
}

// 본문 이미지를 모바일에서도 크게 볼 방법이 있으면 좋겠다
var imgs = document.querySelectorAll('.article-content img')
for(var i = 0 ; i < imgs.length ; i++) {
  var img = imgs[i];

  var link = document.createElement('a');
  link.href = img.src;
  link.target = '_blank';

  img.parentNode.insertBefore(link, img);
  link.parentNode.removeChild(img);
  link.appendChild(img);
}

var pres = document.querySelectorAll("pre");
for(var i = 0 ; i < pres.length ; i++) {
  var pre = pres[i];
  // 줄번호 활성화
  pre.classList.add("line-numbers");

  // 언어를 모르겠으면 기본값이라도 넣어주기
  var code = pre.firstChild;
  var found = false;
  for(var j = 0 ; j < code.classList.length ; j++) {
    var classname = code.classList.item(j);
    if(classname.startsWith("language-")) {
      found = true;
      break;
    }
  }
  if(!found) {
    code.classList.add("language-none");
  }
}
