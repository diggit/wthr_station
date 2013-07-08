function load_values() 
{
	var xmlhttp;
	if (window.XMLHttpRequest)
	{// code for IE7+, Firefox, Chrome, Opera, Safari
		xmlhttp=new XMLHttpRequest();
	}
	else
	{// code for IE6, IE5
		xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
	}
	xmlhttp.onreadystatechange=function()
	{
		if (xmlhttp.readyState==4 && xmlhttp.status==200)
		{	
			var time=new Date()
			var splittered=xmlhttp.responseText.split("\n");
			document.getElementById("Atemp").innerHTML=splittered[0]+" °C";
			document.getElementById("Apress").innerHTML=splittered[1]+" hPa";
			document.getElementById("Ahumi").innerHTML=splittered[2]+" %";
			document.getElementById("stamp").innerHTML="změřeno "+splittered[3]+", aktualizováno "+time.getHours()+":"+leftPad(time.getMinutes(),2)+":"+leftPad(time.getSeconds(),2);
			notify("stamp")
		}
	}
	xmlhttp.open("GET","actual",true);
	xmlhttp.send();
}

window.onload = startUpdating;

function startUpdating()
{
	load_values();//initial load
	setInterval("load_values();",10000);//refresh
}

function notify(el)
{
	var elem = document.getElementById(el);
	elem.setAttribute("style","background-color: #eee; transition: background 0.3s linear;");
	setTimeout(function() {elem.setAttribute("style","background-color: #ccc;  transition: background 0.3s linear;");;},300);
}

function leftPad(number, targetLength) {
    var output = number + '';
    while (output.length < targetLength) {
        output = '0' + output;
    }
    return output;
}