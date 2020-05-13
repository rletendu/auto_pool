function onFilterButton() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "filter_on", true);
    xhttp.send();
}


function getParameters() {
    var xhttp_measures = new XMLHttpRequest();
    xhttp_measures.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            const obj = JSON.parse(this.responseText);
            /*
            document.getElementById("valeurTemperature").innerHTML = obj.water_temperature.toFixed(1);
            document.getElementById("valeurOrp").innerHTML = obj.orp.toFixed(0);
            document.getElementById("valeurPh").innerHTML = obj.ph.toFixed(2);
            document.getElementById("valeurPression").innerHTML = obj.pump_pressure.toFixed(2);
            document.getElementById("valeurSysTemperature").innerHTML = obj.system_temperature.toFixed(1);
            document.getElementById("valeurSysHumidity").innerHTML = obj.system_humidity.toFixed(1);
            document.getElementById("valeurDailyCl").innerHTML = obj.daily_ml_orp;
            document.getElementById("valeurDailyPhm").innerHTML = obj.daily_ml_ph_minus;
        */
        }
    };

    xhttp_measures.open("GET", "getparameters", true);
    xhttp_measures.send();

}

function getData() {
    getMeasures();
    getFilterState();
    getOrpState();
    getPhState();

}

//setInterval(getData, 4000);

document.addEventListener('readystatechange', event => {

    // When HTML/DOM elements are ready:
    /*
     if (event.target.readyState === "interactive") {   //does same as:  ..addEventListener("DOMContentLoaded"..
         alert("hi 1");
     }
     */

    // When window loaded ( external resources are loaded too- `css`,`src`, etc...) 
    if (event.target.readyState === "complete") {
        getParameters();
        
        var timer_val = 16775168;
        var i = 0;
        for (i = 0; i < 24; i++) {
            var s = new String('h') + String(i);
            document.getElementById(s).checked = (timer_val>>i) & 1;
        }

    }
});

/*

{"mqtt_server":"192.168.1.39","mqtt_port":"1883","mqtt_user":"admin","mqtt_pass":"pulsar","base_topic":"autopool","target_ph":7.2,"delta_ph":0.5,"target_orp":650,"delta_orp":50,"flow_cl":20,"flow_ph_minus":20,"flow_ph_plus":20.2,"filter_auto_mode":0,"timer_prog":16775168,"pressure_warning":1.8,"cl_max_day":1000,"phm_max_day":1000,"orp_offset":0,"ph_offset":0}
*/