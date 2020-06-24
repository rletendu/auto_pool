function onFilterButton() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "filter_on", true);
    xhttp.send();
}

function offFilterButton() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "filter_off", true);
    xhttp.send();
}
function autoFilterButton() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "filter_auto", true);
    xhttp.send();
}

function FilterPwrFullButton() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "filter_pwr_full", true);
    xhttp.send();
}

function FilterPwrRegButton() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "filter_pwr_reg", true);
    xhttp.send();
}

function onOrpButton() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "orp_on", true);
    xhttp.send();
}

function offOrpButton() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "orp_off", true);
    xhttp.send();
}
function autoOrpButton() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "orp_auto", true);
    xhttp.send();
}

function onPhButton() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "ph_on", true);
    xhttp.send();
}

function offPhButton() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "ph_off", true);
    xhttp.send();
}
function autoPhButton() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "ph_auto", true);
    xhttp.send();
}

function getMeasures() {
    var xhttp_measures = new XMLHttpRequest();
    xhttp_measures.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            const obj = JSON.parse(this.responseText);
            document.getElementById("valeurTemperature").innerHTML = obj.water_temperature.toFixed(1);
            document.getElementById("valeurOrp").innerHTML = obj.orp.toFixed(0);
            document.getElementById("valeurPh").innerHTML = obj.ph.toFixed(2);
            document.getElementById("valeurPression").innerHTML = obj.pump_pressure.toFixed(2);
            document.getElementById("valeurSysTemperature").innerHTML = obj.system_temperature.toFixed(1);
            document.getElementById("valeurSysHumidity").innerHTML = obj.system_humidity.toFixed(1);
            document.getElementById("valeurDailyCl").innerHTML = obj.daily_ml_orp;
            document.getElementById("valeurDailyPhm").innerHTML = obj.daily_ml_ph_minus;
        }
    };

    xhttp_measures.open("GET", "getmeasures", true);
    xhttp_measures.send();

}
function getFilterState() {
    var xhttp_measures = new XMLHttpRequest();
    xhttp_measures.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            const obj = JSON.parse(this.responseText);
            var mode = "---"
            var pwr = "---"
            switch (obj.filter_mode) {
                case 0:
                    mode = "OFF";
                    break;

                case 1:
                    mode = "ON";
                    break;

                case 2:
                    mode = "AUTO";
                    break;

                default:
                    mode = "---";
            }
            switch (obj.filter_power) {
                case 0:
                    pwr = "FULL";
                    break;

                case 1:
                    pwr = "REG";
                    break;

            }
            document.getElementById("stateFilter").innerHTML = mode;
            if (obj.filter_pump) {
                document.getElementById("PumpFilter").innerHTML = "ON";
                document.getElementById("PumpFilter").className = "w3-green"
            } else {
                document.getElementById("PumpFilter").innerHTML = "OFF";
                document.getElementById("PumpFilter").className = "w3-red"

            }
            
        }
    };
    xhttp_measures.open("GET", "getfilterstate", true);
    xhttp_measures.send();
}

function getOrpState() {
    var xhttp_measures = new XMLHttpRequest();
    xhttp_measures.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            const obj = JSON.parse(this.responseText);
            var mode = "---"
            switch (obj.orp_mode) {
                case 0:
                    mode = "OFF";
                    break;

                case 1:
                    mode = "ON";
                    break;

                case 2:
                    mode = "AUTO";
                    break;

                default:
                    mode = "---";
            }
            document.getElementById("stateOrp").innerHTML = mode;
            if (obj.orp_pump) {
                document.getElementById("PumpOrp").innerHTML = "ON";
                document.getElementById("PumpOrp").className = "w3-green"
            } else {
                document.getElementById("PumpOrp").innerHTML = "OFF";
                document.getElementById("PumpOrp").className = "w3-red"

            }
        }
    };
    xhttp_measures.open("GET", "getorpstate", true);
    xhttp_measures.send();
}


function getPhState() {
    var xhttp_measures = new XMLHttpRequest();
    xhttp_measures.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            const obj = JSON.parse(this.responseText);
            var mode = "---"
            switch (obj.ph_minus_mode) {
                case 0:
                    mode = "OFF";
                    break;

                case 1:
                    mode = "ON";
                    break;

                case 2:
                    mode = "AUTO";
                    break;

                default:
                    mode = "---";
            }
            document.getElementById("statePh").innerHTML = mode;
            if (obj.ph_minus_pump) {
                document.getElementById("PumpPh").innerHTML = "ON";
                document.getElementById("PumpPh").className = "w3-green"
            } else {
                document.getElementById("PumpPh").innerHTML = "OFF";
                document.getElementById("PumpPh").className = "w3-red"

            }
        }
    };
    xhttp_measures.open("GET", "getphstate", true);
    xhttp_measures.send();
}

function getData() {
    getMeasures();
    getFilterState();
    getOrpState();
    getPhState();

}

setInterval(getData, 4000);