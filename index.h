// ================= HTML & CSS =================
// Dipisahkan dari Esp32.ino agar lebih mudah dibaca

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
  <title>TRANSPORTER BOT CONTROLLER</title>
  <style>
    /* --- RESET & ANTI-SELECT (PENTING!) --- */
    * {
      -webkit-touch-callout: none;
      -webkit-user-select: none;
      -khtml-user-select: none;
      -moz-user-select: none;
      -ms-user-select: none;
      user-select: none;
      touch-action: none;
    }

    body { 
      font-family: 'Segoe UI', sans-serif; 
      background-color: #1a1a1a; 
      color: white; 
      margin: 0; padding: 0; 
      overflow: hidden; 
    }
    
    .header-title {
      position: absolute;
      top: 10px;
      width: 100%;
      text-align: center;
      font-size: 18px;
      font-weight: bold;
      color: #f39c12;
      text-shadow: 2px 2px 4px #000;
      z-index: 50;
      pointer-events: none;
    }

    #main-container {
      display: none; 
      width: 100vw; height: 100vh;
      flex-direction: row; justify-content: space-between; align-items: center;
      padding: 0 40px; box-sizing: border-box;
      padding-top: 30px;
    }

    #fs-toggle {
      position: absolute;
      top: 10px; right: 10px;
      background: #333; border: 1px solid #555;
      color: white; border-radius: 5px;
      width: 35px; height: 35px;
      font-size: 20px; cursor: pointer;
      z-index: 100;
      pointer-events: auto;
    }

    .control-pad { display: grid; grid-template-columns: repeat(3, 80px); grid-template-rows: repeat(3, 80px); gap: 5px; }
    
    .speed-control { 
      display: flex; flex-direction: column; align-items: center; justify-content: center; 
      gap: 10px; width: 200px; 
      background: rgba(255,255,255,0.05); padding: 15px; border-radius: 15px;
    }
    input[type=range] { -webkit-appearance: none; width: 100%; height: 15px; border-radius: 5px; background: #555; outline: none; pointer-events: auto; }
    input[type=range]::-webkit-slider-thumb { -webkit-appearance: none; appearance: none; width: 25px; height: 25px; border-radius: 50%; background: #3498db; cursor: pointer; }
    
    .action-pad { display: grid; grid-template-columns: repeat(3, 80px); grid-template-rows: repeat(3, 80px); gap: 5px; }

    button { 
      border-radius: 12px; border: none; font-size: 28px; cursor: pointer; color: white; 
      box-shadow: 0 4px 6px rgba(0,0,0,0.3); transition: transform 0.1s; 
      -webkit-tap-highlight-color: transparent;
      pointer-events: auto;
    }
    button:active { transform: scale(0.95); box-shadow: 0 2px 3px rgba(0,0,0,0.2); }
    
    .btn-move { width: 80px; height: 80px; background: linear-gradient(145deg, #3498db, #2980b9); }
    .btn-stop { width: 80px; height: 80px; background: linear-gradient(145deg, #e74c3c, #c0392b); font-size: 20px; }
    .btn-servo { width: 80px; height: 80px; background: linear-gradient(145deg, #2ecc71, #27ae60); font-size: 16px; font-weight: bold; }

    .btn-calib {
      width: 80px; height: 80px;
      background: linear-gradient(145deg, #f39c12, #d35400); 
      font-size: 16px; font-weight: bold; letter-spacing: 1px;
    }

    .up { grid-column: 2; grid-row: 1; }
    .left { grid-column: 1; grid-row: 2; }
    .stop { grid-column: 2; grid-row: 2; }
    .right { grid-column: 3; grid-row: 2; }
    .down { grid-column: 2; grid-row: 3; }

    .servo-up { grid-column: 2; grid-row: 1; }
    .servo-left { grid-column: 1; grid-row: 2; }
    .servo-center { grid-column: 2; grid-row: 2; }
    .servo-right { grid-column: 3; grid-row: 2; }
    .servo-down { grid-column: 2; grid-row: 3; }

    #portrait-warning {
      position: fixed; top: 0; left: 0; width: 100%; height: 100%;
      background: #000; display: flex; flex-direction: column;
      justify-content: center; align-items: center; z-index: 9999;
    }
    #start-btn { padding: 15px 30px; font-size: 20px; background: #27ae60; border: none; border-radius: 10px; color: white; margin-top: 20px; pointer-events: auto;}

    @media screen and (orientation: landscape) {
      #portrait-warning { display: none; }
      #main-container { display: flex; }
    }
    @media screen and (orientation: portrait) {
      #portrait-warning { display: flex; }
      #main-container { display: none; }
    }
  </style>
</head>
<body>

  <div id="portrait-warning">
    <h1>⚠️ ROTATE DEVICE ⚠️</h1>
    <p>Please turn your phone to LANDSCAPE</p>
    <button id="start-btn" onclick="goFullscreen()">START OPREC RDC</button>
  </div>

  <div class="header-title">
    REMOTE CONTROLLER<br>OPREC RDC UAD
  </div>
  
  <button id="fs-toggle" onclick="toggleFullscreen()">&#x26F6;</button>

  <div id="main-container">
    
    <div class="control-pad">
      <button class="btn-move up" ontouchstart="send('F')" ontouchend="send('P')">&#9650;</button>
      <button class="btn-move left" ontouchstart="send('R')" ontouchend="send('P')">&#9664;</button> 
      <button class="btn-stop stop" onclick="send('P')">&#9632;</button> 
      <button class="btn-move right" ontouchstart="send('L')" ontouchend="send('P')">&#9654;</button> 
      <button class="btn-move down" ontouchstart="send('B')" ontouchend="send('P')">&#9660;</button> 
    </div>

    <div class="speed-control">
      <label>MOTOR SPEED</label>
      <input type="range" min="0" max="255" value="140" id="pwmSlider" oninput="updateSpeed(this.value)" onchange="sendSpeed(this.value)">
      <label style="font-size: 20px; font-weight: bold;" id="speed-val">140</label>
    </div>

    <div class="action-pad">
      <button class="btn-servo servo-up" onclick="send('T')">LIFT<br>&#9650;</button>
      <button class="btn-servo servo-left" onclick="send('C')">GRIP<br>OPEN</button>
      <button class="btn-calib servo-center" onclick="send('A')">CALIB</button>
      <button class="btn-servo servo-right" onclick="send('S')">GRIP<br>CLOSE</button>
      <button class="btn-servo servo-down" onclick="send('X')">LIFT<br>&#9660;</button>
    </div>

  </div>

<script>
  document.addEventListener('contextmenu', event => event.preventDefault());

  function goFullscreen() {
    var elem = document.documentElement
    if (elem.requestFullscreen) { elem.requestFullscreen(); }
    else if (elem.webkitRequestFullscreen) { elem.webkitRequestFullscreen(); }
  }

  function toggleFullscreen() {
    if (!document.fullscreenElement) {
      goFullscreen();
    } else {
      if (document.exitFullscreen) { document.exitFullscreen(); }
    }
  }

  function send(cmd) {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/cmd?val=" + cmd, true);
    xhr.send();
  }

  function updateSpeed(val) {
    document.getElementById('speed-val').innerText = val;
  }

  function sendSpeed(val) {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/speed?val=" + val, true);
    xhr.send();
  }
</script>
</body>
</html>
)rawliteral";
