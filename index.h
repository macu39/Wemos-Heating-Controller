const char MAIN_page[] PROGMEM = R"=====(
<html>
    <head>
        <title>Calefaccion</title>
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <style>

            @import url('https://fonts.googleapis.com/css?family=Roboto:100,300');

            body{
                font-family: 'Roboto';
                margin: 0px;
                background: #283044;
            }

            .container{
                width: 400px;
                margin:50px auto;
                text-align:center;
                background: white;
                border-radius: 30px;
            }

            @media only screen and (max-width: 400px) {

                body{
                    background: white;
                }
                
                .container{
                    width: 100%;
                    margin: 0px;
                    border-radius: 0px;
                }

            }

            .presets{
                padding: 10px;
                Margin: 10px 0px;
            }

            button{
                padding: 10px;
                width: calc(50% - 15px);
                border: 0px;
                margin: 5px;
                font-size: 17px;
                background: #78A1BB;
                color: white;
                border-radius: 10px;
            }

            button:hover{
                background: #283044;
            }

            .stop{

                width: 70%;
                background: #F76560;

            }

            h1{
                font-weight: 10;
                margin: 0px 0px -20px 0px;
                font-size: 65px;
                padding-top: 10px;
                color: #78A1BB;
            }

            /* Counter */

            .counter {
                position: relative;
                top: 30px;
                width: 300px;
                margin: 0 auto 30px auto;
                display: none;
            }

            .controlls {
                position: absolute;
                left: 75px;
                top: 105px;
                text-align: center;
            }

            .display-remain-time {
                font-family: 'Roboto';
                font-weight: 100;
                font-size: 65px;
                color: #78A1BB;
            }

            #pause {
                outline: none;
                background: transparent;
                border: none;
                margin-top: 10px;
                width: 50px;
                height: 50px;
                position: relative;
            }

            .play::before {
                display: block;
                content: "";
                position: absolute;
                top: 8px;
                left: 16px;
                border-top: 15px solid transparent;
                border-bottom: 15px solid transparent;
                border-left: 22px solid #78A1BB;
            }

            .pause::after {
                content: "";
                position: absolute;
                top: 8px;
                left: 12px;
                width: 15px;
                height: 30px;
                background-color: transparent;
                border-radius: 1px;
                border: 5px solid #78A1BB;
                border-top: none;
                border-bottom: none;
            }

            #pause:hover {
                opacity: 0.8;
            }

            .e-c-base {
                fill: none;
                stroke: #B6B6B6;
                stroke-width: 4px
            }

            .e-c-progress {
                fill: none;
                stroke: #78A1BB;
                stroke-width: 4px;
                transition: stroke-dashoffset 0.7s;
            }

            .e-c-pointer {
                fill: #FFF;
                stroke: #78A1BB;
                stroke-width: 2px;
            }

            #e-pointer {
                transition: transform 0.7s;
            }

        </style>
    </head>
    <body>
        <div class="container">
            <h1>Calefaccion</h1>
            <div id="counter" class="counter">
                <div class="circle"> 
                    <svg width="300" viewBox="0 0 220 220" xmlns="http://www.w3.org/2000/svg">
                        <g transform="translate(110,110)">
                            <circle r="100" class="e-c-base" />
                            <g transform="rotate(-90)">
                                <circle r="100" class="e-c-progress" />
                                <g id="e-pointer">
                                    <circle cx="100" cy="0" r="8" class="e-c-pointer" />
                                </g>
                            </g>
                        </g>
                    </svg> 
                </div>
                <div class="controlls">
                    <div class="display-remain-time">00:30</div>
                    <button id="stop" class="stop">Apagar</button>
                </div>
            </div>
            <div class="presets">        
                <button class="start" data="5">5 min</button></a>
                <button class="start" data="10">10 min</button></a>
                <button class="start" data="15">15 min</button></a> 
                <button class="start" data="20">20 min</button></a>
                <button class="start" data="25">25 min</button></a>
                <button class="start" data="30">30 min</button></a>
                <button class="start" data="45">45 min</button></a>
                <button class="start" data="60">1 h</button></a>                     
            </div>
        </div>
        <script>
            //circle start
            let intervalTimer;
            let progressBar = document.querySelector('.e-c-progress');
            let indicator = document.getElementById('e-indicator');
            let pointer = document.getElementById('e-pointer');
            let counter = document.getElementById('counter');
            let stopButton = document.getElementById('stop');
            let presetButton = document.getElementById('start');
            let length = Math.PI * 2 * 100;
            progressBar.style.strokeDasharray = length;
    
            //circle ends
            const displayOutput = document.querySelector('.display-remain-time')
            const pauseBtn = document.getElementById('pause');
            const setterBtns = document.querySelectorAll('button[data-setter]');
    
            function start(time) {

                let req = new XMLHttpRequest();
                req.open('GET', '/on?t='+time, false); 
                req.send();
                if (req.status == 200){

                    stop();
    
                    let wholeTime = time * 60;
                    counter.style.display = 'block';
                    update(wholeTime, wholeTime); //refreshes progress bar
                    displayTimeLeft(wholeTime, wholeTime);
                    timer(wholeTime);

                }
    
            }
    
            function hardStop() {

                let req = new XMLHttpRequest();
                req.open('GET', '/on?t=0', false); 
                req.send();
                if (req.status == 200){
    
                    stop();

                }
    
            }
            
            function stop() {

                counter.style.display = 'none';
                clearInterval(intervalTimer);
                displayTimeLeft(0, 0);

            }
    
            function timer(seconds) {
    
                let remainTime = (Date.now() + (seconds * 1000));
    
                intervalTimer = setInterval(function () {
    
                    let timeLeft = Math.round((remainTime - Date.now()) / 1000);
                    if (timeLeft < 0) {
    
                        stop();
    
                    } else {
    
                        displayTimeLeft(timeLeft, seconds);
    
                    }
    
                }, 1000);
    
            }
    
            function update(value, timePercent) {
    
                var offset = -length - length * value / (timePercent);
                progressBar.style.strokeDashoffset = offset;
                pointer.style.transform = `rotate(${360 * value / (timePercent)}deg)`;
    
            };
    
            function displayTimeLeft(timeLeft, wholeTime) {
    
                let minutes = Math.floor(timeLeft / 60);
                let seconds = timeLeft % 60;
                let displayString = `${minutes < 10 ? '0' : ''}${minutes}:${seconds < 10 ? '0' : ''}${seconds}`;
                displayOutput.textContent = displayString;
                update(timeLeft, wholeTime);
    
            }
        
            function init(){

                let req = new XMLHttpRequest();
                req.open('GET', '/time', false); 
                req.send();
                if (req.status == 200){

                    let response = JSON.parse(req.responseText);
                    let time = response.time;
                    if(time > 0){ start(time); }

                }               

            }

            stopButton.addEventListener("click", hardStop);
            const predefinidos = document.querySelectorAll('.start');
            predefinidos.forEach(element => {
                element.addEventListener("click", function(event){
                    start(this.getAttribute("data"));
                });

            });

            init();

        </script>
    </body>
</html>
)=====";
