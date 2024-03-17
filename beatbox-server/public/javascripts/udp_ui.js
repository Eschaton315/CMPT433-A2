"use strict";
// Client-side interactions with the browser.

// Make connection to server when web page is fully loaded.
var socket = io.connect();
$(document).ready(function() {
	
	displayError(false);
	
	sendPrimeCommand("getvolume");
	socket.on('commandReply', function(result) {
		var newDiv = $('<div></div>').text(result);
		$('#volumeid').replaceWith(newDiv);
		
	});
	
	sendPrimeCommand("getstatus");
	socket.on('commandReply', function(result) {
		var newDiv = $('<div></div>').text(result);
		$('#status').replaceWith(newDiv);
		
	});
	
	sendPrimeCommand("gettempo");
	socket.on('commandReply', function(result) {
		var newDiv = $('<div></div>').text(result);
		$('#tempoid').replaceWith(newDiv);
		
	});
		
	
	$('#modeNone').click(function(){
		sendPrimeCommand("silence");
		socket.on('commandReply', function(result) {
			var newDiv = $('<div></div>').text(result);
			$('#modeid').replaceWith(newDiv);
		});
	});
	$('#modeRock1').click(function(){
		sendPrimeCommand("rock");
		socket.on('commandReply', function(result) {
			var newDiv = $('<div></div>').text(result);
			$('#modeid').replaceWith(newDiv);
		});
	});
	$('#customBeat').click(function(){
		sendPrimeCommand("custombeat");
		socket.on('commandReply', function(result) {
			var newDiv = $('<div></div>').text(result);
			$('#modeid').replaceWith(newDiv);
		});
	});
	$('#volumeDown').click(function(){
		decreaseVolume();
		socket.on('commandReply', function(result) {
			var newDiv = $('<div></div>').text(result);
			$('#volumeid').replaceWith(newDiv);
		});
	});
	$('#volumeUp').click(function(){
		increaseVolume();
		socket.on('commandReply', function(result) {
			var newDiv = $('<div></div>').text(result);
			$('#volumeid').replaceWith(newDiv);
		});
	});
	$('#tempoDown').click(function(){
		decreaseVolume();
		socket.on('commandReply', function(result) {
			var newDiv = $('<div></div>').text(result);
			$('#tempoid').replaceWith(newDiv);
		});
	});
	$('#tempoUp').click(function(){
		increaseVolume();
		socket.on('commandReply', function(result) {
			var newDiv = $('<div></div>').text(result);
			$('#tempoid').replaceWith(newDiv);
		});
	});	
	$('#stop').click(function(){
		sendPrimeCommand("stop");
		socket.on('commandReply', function(result) {
			var newDiv = $('<div></div>').text(result);
			$('#tempoid').replaceWith(newDiv);
		});
	});
	
});

function displayError(display){
	if(display == false){
		var errorBox = document.getElementById('error-box');
		errorBox.style.display = 'none';
		
	}else{
		var errorBox = document.getElementById('error-box');
		errorBox.style.display = 'block';
	}
	
}

function decreaseVolume() {
	var volume = document.getElementById('volumeid').value;
	
	if(volume > 4 ){
		volume = volume - 5;
		sendPrimeCommand("changevol" + volume)
	}
	
}

function increaseVolume() {
	var volume = document.getElementById('volumeid').value;
	
	if(volume < 96 ){
		volume = volume + 5;
		sendPrimeCommand("changevol" + volume)
	}
	
}

function increaseTempo() {
	var tempo = document.getElementById('tempoid').value;
	
	if(tempo < 296 ){
		tempo = tempo + 5;
		sendPrimeCommand("changetempo" + tempo)
	}	
}

function decreaseTempo() {
	var tempo = document.getElementById('tempoid').value;
	
	if(tempo > 44 ){
		tempo = tempo - 5;
		sendPrimeCommand("changetempo" + tempo)
	}	
}

function sendPrimeCommand(message) {
	socket.emit('prime', message);
};