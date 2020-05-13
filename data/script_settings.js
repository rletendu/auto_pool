const dummy_param_str = '{"mqtt_server":"192.168.1.39","mqtt_port":"1883","mqtt_user":"admin","mqtt_pass":"pulsar","base_topic":"autopool","target_ph":7.2,"delta_ph":0.5,"target_orp":650,"delta_orp":50,"flow_cl":20,"flow_ph_minus":20,"flow_ph_plus":20.2,"filter_auto_mode":0,"timer_prog":16775168,"pressure_warning":1.8,"cl_max_day":1000,"phm_max_day":1000,"orp_offset":0,"ph_offset":0.1}'
var json_param =  JSON.parse(dummy_param_str);

function onFilterButton() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "filter_on", true);
    xhttp.send();
}


function getParameters() {
    var xhttp_measures = new XMLHttpRequest();
    xhttp_measures.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
           
            json_param = JSON.parse(this.responseText);
            json_param_2_page();
        }
    };
    json_param_2_page();

    xhttp_measures.open("GET", "getparameters", true);
    xhttp_measures.send();


}

function getData() {
    getMeasures();
    getFilterState();
    getOrpState();
    getPhState();

}

function json_param_2_page() {
    document.getElementById("target_ph").value = json_param.target_ph;
    document.getElementById("delta_ph").value = json_param.delta_ph;
    document.getElementById("target_orp").value = json_param.target_orp;
    document.getElementById("delta_orp").value = json_param.delta_orp;
    document.getElementById("flow_cl").value = json_param.flow_cl;
    document.getElementById("flow_ph_minus").value = json_param.flow_ph_minus;
    document.getElementById("pressure_warning").value = json_param.pressure_warning;
    document.getElementById("cl_max_day").value = json_param.cl_max_day;
    document.getElementById("phm_max_day").value = json_param.phm_max_day;
    document.getElementById("orp_offset").value = json_param.orp_offset;
    document.getElementById("ph_offset").value = json_param.ph_offset;
    var i = 0;
    for (i = 0; i < 24; i++) {
        var s = new String('h') + String(i);
        document.getElementById(s).checked = (json_param.timer_prog >> i) & 1;
    }
}

function page_2_json_param() {
    json_param.target_ph = document.getElementById("target_ph").value;
    json_param.delta_ph = document.getElementById("delta_ph").value;
    json_param.target_orp = document.getElementById("target_orp").value;
    json_param.delta_orp = document.getElementById("delta_orp").value;
    json_param.flow_cl = document.getElementById("flow_cl").value;
    json_param.flow_ph_minus = document.getElementById("flow_ph_minus").value;
    json_param.pressure_warning = document.getElementById("pressure_warning").value;
    json_param.cl_max_day = document.getElementById("cl_max_day").value;
    json_param.phm_max_day = document.getElementById("phm_max_day").value;
    json_param.orp_offset = document.getElementById("orp_offset").value;
    json_param.ph_offset = document.getElementById("ph_offset").value;
    var i = 0;
    for (i = 0; i < 24; i++) {
        var s = new String('h') + String(i);
        json_param.timer_prog +=  document.getElementById(s).checked<<i;
    }
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
            document.getElementById(s).checked = (timer_val >> i) & 1;
        }

    }
});


/*

{"mqtt_server":"192.168.1.39","mqtt_port":"1883","mqtt_user":"admin","mqtt_pass":"pulsar","base_topic":"autopool","target_ph":7.2,"delta_ph":0.5,"target_orp":650,"delta_orp":50,"flow_cl":20,"flow_ph_minus":20,"flow_ph_plus":20.2,"filter_auto_mode":0,"timer_prog":16775168,"pressure_warning":1.8,"cl_max_day":1000,"phm_max_day":1000,"orp_offset":0,"ph_offset":0}
*/