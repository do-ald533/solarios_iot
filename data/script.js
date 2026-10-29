let socket;
let activeTimerInterval = null;
let visitStartSeconds = 0;

function connectWebSocket() {
    socket = new WebSocket(`ws://${window.location.hostname}/ws`);

    socket.onopen = () => {
        document.getElementById('conn-status').className = 'badge connected';
        document.getElementById('conn-status').innerText = 'Connected';
    };

    socket.onclose = () => {
        document.getElementById('conn-status').className = 'badge disconnected';
        document.getElementById('conn-status').innerText = 'Disconnected';
        setTimeout(connectWebSocket, 2000);
    };

    socket.onmessage = (event) => {
        const data = JSON.parse(event.data);
        updateDashboard(data);
    };
}

function updateDashboard(data) {
    // Occupancy Status
    const stateEl = document.getElementById('occupancy-state');
    if (data.occupied) {
        stateEl.className = 'state-box occupied';
        stateEl.innerText = 'OCCUPIED';
        syncActiveTimer(data.activeVisitSec || 0);
    } else {
        stateEl.className = 'state-box vacant';
        stateEl.innerText = 'VACANT';
        stopActiveTimer();
    }

    // Telemetry & Metrics
    document.getElementById('visit-count').innerText = data.visitCount;
    document.getElementById('wifi-rssi').innerText = `${data.rssi} dBm`;
    document.getElementById('free-heap').innerText = `${Math.round(data.freeHeap / 1024)} KB`;
    document.getElementById('system-uptime').innerText = formatUptime(data.uptimeSec);
    document.getElementById('last-sync').innerText = data.timestamp;

    // Controls Feedback
    document.getElementById('timeout-slider').value = data.ledTimeoutSec;
    document.getElementById('timeout-val').innerText = data.ledTimeoutSec;

    document.querySelectorAll('.mode-btn').forEach(btn => {
        btn.classList.toggle('active', btn.dataset.mode === data.ledMode);
    });
}

function sendCommand(cmd, value) {
    if (socket && socket.readyState === WebSocket.OPEN) {
        socket.send(JSON.stringify({ command: cmd, value: value }));
    }
}

// UI Event Listeners
document.querySelectorAll('.mode-btn').forEach(btn => {
    btn.addEventListener('click', (e) => sendCommand('setLedMode', e.target.dataset.mode));
});

document.getElementById('timeout-slider').addEventListener('change', (e) => {
    document.getElementById('timeout-val').innerText = e.target.value;
    sendCommand('setTimeout', parseInt(e.target.value));
});

function syncActiveTimer(serverSeconds) {
    visitStartSeconds = serverSeconds;
    renderTimerDisplay(visitStartSeconds);

    if (!activeTimerInterval) {
        activeTimerInterval = setInterval(() => {
            visitStartSeconds++;
            renderTimerDisplay(visitStartSeconds);
        }, 1000);
    }
}

function renderTimerDisplay(totalSeconds) {
    const mins = String(Math.floor(totalSeconds / 60)).padStart(2, '0');
    const secs = String(totalSeconds % 60).padStart(2, '0');
    document.getElementById('active-timer').innerText = `${mins}:${secs}`;
}

function stopActiveTimer() {
    if (activeTimerInterval) {
        clearInterval(activeTimerInterval);
        activeTimerInterval = null;
    }
    document.getElementById('active-timer').innerText = '00:00';
}

function formatUptime(sec) {
    const d = Math.floor(sec / 86400);
    const h = Math.floor((sec % 86400) / 3600);
    const m = Math.floor((sec % 3600) / 60);
    return `${d}d ${h}h ${m}m`;
}

window.addEventListener('load', connectWebSocket);