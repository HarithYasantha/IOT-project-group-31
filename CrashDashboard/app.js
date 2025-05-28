import { initializeApp } from "https://www.gstatic.com/firebasejs/9.22.2/firebase-app.js";
import { getDatabase, ref, onValue } from "https://www.gstatic.com/firebasejs/9.22.2/firebase-database.js";

const firebaseConfig = {
  apiKey: "AIzaSyAxRvMXqnP5O5Pf4bEddbsvx-FDjBkoN1w",
  authDomain: "crashalertsystem-1c13f.firebaseapp.com",
  databaseURL: "https://crashalertsystem-1c13f-default-rtdb.asia-southeast1.firebasedatabase.app",
  projectId: "crashalertsystem-1c13f",
  storageBucket: "crashalertsystem-1c13f.appspot.com",
  messagingSenderId: "663104449163",
  appId: "1:663104449163:web:3bc4fae03c835d616ec28e"
};

const app = initializeApp(firebaseConfig);
const db = getDatabase(app);

const crashList = document.getElementById('crashList');

onValue(ref(db, 'crashes'), (snapshot) => {
  crashList.innerHTML = '';

  snapshot.forEach((childSnapshot) => {
    const data = childSnapshot.val();
    const timestamp = new Date(Number(childSnapshot.key)).toLocaleString();
    const li = document.createElement('li');
    li.textContent = `📅 ${timestamp} | ⚡ Accel: ${data.acceleration} m/s²`;
    crashList.appendChild(li);
  });
});
