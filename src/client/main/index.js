"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const electron_1 = require("electron");
const createWindow = () => {
    const win = new electron_1.BrowserWindow({
        width: 800,
        height: 600,
        title: 'Clear vision'
    });
    win.loadURL('http://localhost:5173');
};
electron_1.app.whenReady().then(() => {
    createWindow();
});
//# sourceMappingURL=index.js.map