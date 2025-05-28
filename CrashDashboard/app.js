import { initializeApp } from "https://www.gstatic.com/firebasejs/9.23.0/firebase-app.js";
import { getDatabase, ref, onValue } from "https://www.gstatic.com/firebasejs/9.23.0/firebase-database.js";
import { getAuth, signInAnonymously, onAuthStateChanged } from "https://www.gstatic.com/firebasejs/9.23.0/firebase-auth.js";

// Firebase config
const firebaseConfig = {
  apiKey: "AIzaSyAxRvMXqnP5O5Pf4bEddbsvx-FDjBkoN1w",
  authDomain: "crashalertsystem-1c13f.firebaseapp.com",
  databaseURL: "https://crashalertsystem-1c13f-default-rtdb.asia-southeast1.firebasedatabase.app",
  projectId: "crashalertsystem-1c13f",
  storageBucket: "crashalertsystem-1c13f.appspot.com",
  messagingSenderId: "663104449163",
  appId: "1:663104449163:web:3bc4fae03c835d616ec28e"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const db = getDatabase(app);
const auth = getAuth(app);

// DOM element
const crashList = document.getElementById('crashList');

// Sign in anonymously
signInAnonymously(auth)
  .then(() => {
    console.log("✅ Signed in anonymously");

    // Now safe to listen to DB
    const crashesRef = ref(db, 'crashes');
    onValue(crashesRef, (snapshot) => {
      crashList.innerHTML = '';
      snapshot.forEach(childSnapshot => {
        const data = childSnapshot.val();
        const li = document.createElement('li');
        li.textContent = `Time: ${new Date(+childSnapshot.key).toLocaleString()} | Accel: ${data.acceleration} m/s²`;
        crashList.appendChild(li);
      });
    });

  })
  .catch((error) => {
    console.error("❌ Auth error:", error);
  });
