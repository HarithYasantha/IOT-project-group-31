import { initializeApp } from "https://www.gstatic.com/firebasejs/9.22.2/firebase-app.js";
import { getDatabase, ref, onValue } from "https://www.gstatic.com/firebasejs/9.22.2/firebase-database.js";

// Your Firebase config
const firebaseConfig = {
  apiKey: "AIzaSyAxRvMXqnP5O5Pf4bEddbsvx-FDjBkoN1w",
  databaseURL: "https://crashalertsystem-1c13f-default-rtdb.asia-southeast1.firebasedatabase.app"
};

const app = initializeApp(firebaseConfig);
const db = getDatabase(app);
const crashList = document.getElementById('crashList');

onValue(ref(db, '/crashes'), (snapshot) => {
  crashList.innerHTML = '';
  snapshot.forEach(child => {
    const data = child.val();
    const li = document.createElement('li');
    li.textContent = `Time: ${new Date(+child.key).toLocaleString()} | Accel: ${data.acceleration} m/s²`;
    crashList.appendChild(li);
  });
});
