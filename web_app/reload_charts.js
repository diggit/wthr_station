charts_interval=60000;

function charts_begin(){
	setInterval("reload_image('charts/temp-lowres.png','temp_img')", charts_interval); // 30 seconds refresh
	setInterval("reload_image('charts/pres-lowres.png','pres_img')", charts_interval); // 30 seconds refresh
	setInterval("reload_image('charts/humi-lowres.png','humi_img')", charts_interval); // 30 seconds refresh
}


function reload_image(path,ID) {
	var buffer = {};
	//console.info(path,ID)
    buffer = new Image();
    // attaching the seconds breaks cache
    buffer.src = path + '?' + (new Date()).getTime();
    buffer.onload = function() {
        document.getElementById(ID).src = buffer.src;
    }
}