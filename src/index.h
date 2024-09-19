
// HTML and JS code for firmware upload page of site.
const char* serverIndex = 
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>progress: 0%</div>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')" 
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>";




// HTML and JS Code for login page of site
const char* loginIndex = 
  "<form name='loginForm'>"
  "<table width='20%' bgcolor='A09F9F' align='center'>"
  "<tr><td colspan=2><center><font size=4><b>ESP32 Login Page</b></font></center><br></td></tr>"
  "<tr><td>Username:</td><td><input type='text' size=25 name='userid'><br></td></tr>"
  "<tr><td>Password:</td><td><input type='Password' size=25 name='pwd'><br></td></tr>"
  "<tr><td><input type='submit' onclick='check(this.form)' value='Login'></td></tr>"
  "</table></form>"
  "<script>function check(form){if(form.userid.value=='admin' && form.pwd.value=='admin'){window.open('/serverIndex')}"
  "else { alert('Error Password or Username') }} </script>";



// ---------------------------IGNORE FOR NOW---------------------------
//   <form name="loginForm" onsubmit="return check(this);">
//     <table width="20%" bgcolor="#A09F9F" align="center">
//       <tr>
//         <td colspan="2">
//           <center>
//             <font size="4"><b>ESP32 Login Page</b></font>
//           </center>
//           <br>
//         </td>
//       </tr>
//       <tr>
//         <td>Username:</td>
//         <td><input type="text" size="25" name="userid" required><br></td>
//       </tr>
//       <tr>
//         <td>Password:</td>
//         <td><input type="password" size="25" name="pwd" required><br></td>
//       </tr>
//       <tr>
//         <td colspan="2" align="center">
//           <input type="submit" value="Login">
//         </td>
//       </tr>
//     </table>
//   </form>
//   <script>
//     function check(form) {
//       if (form.userid.value === 'admin' && form.pwd.value === 'admin') {
//         window.open('/serverIndex');
//         return false; // Prevent form submission
//       } else {
//         alert('Error: Invalid Username or Password');
//         return false; // Prevent form submission
//       }
//     }
//   </script>



//   <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js"></script>
// <form method="POST" action="#" enctype="multipart/form-data" id="upload_form">
//     <input type="file" name="update">
//     <input type="submit" value="Update">
// </form>

// <div id="prg">Progress: 0%</div>
// <script>
//     $(document).ready(function() {
//         $('#upload_form').on('submit', function(e) {
//             e.preventDefault();
//             var form = this;
//             var data = new FormData(form);
//             $.ajax({
//                 url: '/update',
//                 type: 'POST',
//                 data: data,
//                 contentType: false,
//                 processData: false,
//                 xhr: function() {
//                     var xhr = new window.XMLHttpRequest();
//                     xhr.upload.addEventListener('progress', function(evt) {
//                         if (evt.lengthComputable) {
//                             var per = evt.loaded / evt.total;
//                             $('#prg').html('Progress: ' + Math.round(per * 100) + '%');
//                         }
//                     }, false);
//                     return xhr;
//                 },
//                 success: function(response) {
//                     console.log('Success!', response);
//                 },
//                 error: function(xhr, status, error) {
//                     console.error('Error:', error);
//                 }
//             });
//         });
//     });
// </script>
