import {app, BrowserWindow} from 'electron';

const createWindow = () => {
    const win = new BrowserWindow({
        width: 800,
        height: 600,
        title: 'Clear vision'
    })

    win.loadURL('http://localhost:3000');
}

app.whenReady().then(() => {
    createWindow();
});