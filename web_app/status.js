// document.getElementById("status_container").onclick("toggle_visibility('status_extended')");

function toggle_visibility(id) {
	var e = document.getElementById(id);
	if(e.style.display == 'block')
		e.style.display = 'none';
	else
		e.style.display = 'block';
	AdjustHeight();
}