var code = '1234';

var fb = new Firebase('https://8tracks-pebble.firebaseio.com/codes/' + code);

var first = true; // so we don't get one unless it's new

fb.endAt().limitToLast(1).on('child_added', function(snapshot) {
  if(!first) {
    handleMessage(snapshot.val());
  } else {
    first = false;
  }
});


var handleMessage = function(data) {
  var msg = data.message;
  switch(msg) {
    case "play_page":
      $('#play_overlay')[0].click();
      break;
    case "toggle":
      $('#player_play_button').click();
      break;
    case "skip":
      $('#player_skip_button').click();
      break;
    default:
      console.log("Sorry, that message isn't recognized.");
      break;
  }
};
