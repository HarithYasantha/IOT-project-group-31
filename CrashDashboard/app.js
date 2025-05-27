// Import Firebase functions needed
import { initializeApp } from "https://www.gstatic.com/firebasejs/9.23.0/firebase-app.js";
import {
	getDatabase,
	ref,
	onValue,
} from "https://www.gstatic.com/firebasejs/9.23.0/firebase-database.js";

// Your Firebase config object
const firebaseConfig = {
	apiKey: "AIzaSyAxRvMXqnP5O5Pf4bEddbsvx-FDjBkoN1w",
	authDomain: "crashalertsystem-1c13f.firebaseapp.com",
	databaseURL:
		"https://crashalertsystem-1c13f-default-rtdb.asia-southeast1.firebasedatabase.app",
	projectId: "crashalertsystem-1c13f",
	storageBucket: "crashalertsystem-1c13f.appspot.com",
	messagingSenderId: "663104449163",
	appId: "1:663104449163:web:3bc4fae03c835d616ec28e",
};

// Initialize Firebase app and database
const app = initializeApp(firebaseConfig);
const db = getDatabase(app);

// Select the HTML list element
const crashList = document.getElementById("crashList");

// Reference the "crashes" node in your Realtime Database
const crashesRef = ref(db, "crashes");

// Listen for value changes and update UI
onValue(crashesRef, (snapshot) => {
	console.log("Fetched data:", snapshot.val());
	crashList.innerHTML = "";
	snapshot.forEach((childSnapshot) => {
		const data = childSnapshot.val();
		const li = document.createElement("li");
		li.textContent = `Time: ${new Date(
			+childSnapshot.key
		).toLocaleString()} | Accel: ${data.acceleration} m/s²`;
		crashList.appendChild(li);
	});
});
