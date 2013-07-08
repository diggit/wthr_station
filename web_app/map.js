function initialize() {
	var map_canvas = document.getElementById('map_canvas');
	var map_options = {
		center: new google.maps.LatLng(50.043,14.5025),
		zoom: 15,
		mapTypeId: google.maps.MapTypeId.ROADMAP
	}
	var map = new google.maps.Map(map_canvas, map_options)

	// Construct the circle for each value in citymap. We scale population by 20.
	var populationOptions = {
		strokeColor: '#55F',
		strokeOpacity: 0.8,
		strokeWeight: 2,
		fillColor: '#55F',
		fillOpacity: 0.35,
		map: map,
		center: map_options.center,
		radius: 100
	};
	cityCircle = new google.maps.Circle(populationOptions);



}
function extended_init(){
	startUpdating();
	initialize()
}
window.onload = extended_init;