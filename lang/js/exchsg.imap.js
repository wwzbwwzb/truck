var util = require('util'),
ImapConnection = require('imap').ImapConnection;

var imap = new ImapConnection({
 username: 'johneyguo@viatech.com.cn',
 password: '1',
 host: 'exchsg04.s3graphics.com',
 port: 993,
 secure: true
});

function show(obj) {
  return util.inspect(obj, false, Infinity);
}

function die(err) {
  console.log('Uh oh: ' + err);
  process.exit(1);
}

function openInbox(cb) {
  console.log(' connect to imap sever ');
  imap.connect(function(err) {
      console.log(' connecting');
      if (err) die(err);
      console.log(' open INBOX');
      imap.openBox('INBOX', false, cb);
      });
}

openInbox(function(err, mailbox) {
    if (err) die(err);
    imap.search([ 'UNSEEN', ['SINCE', 'May 20, 2010'] ], function(err, results) {
      if (err) die(err);
      var fetch = imap.fetch(results, {
        request: {
        headers: ['from', 'to', 'subject', 'date']
        }
      });
      fetch.on('message', function(msg) {
        console.log('Got a message with sequence number ' + msg.seqno);
        msg.on('end', function() {
          // msg.headers is now an object containing the requested headers ...
          console.log('Finished message. Headers ' + show(msg.headers));
          });
        });
      fetch.on('end', function() {
        console.log('Done fetching all messages!');
        imap.logout();
        });
      });
});
