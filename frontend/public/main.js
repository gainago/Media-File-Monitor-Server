"use strict";
const tbody = document.querySelector('#media-table tbody');
const source = new EventSource('/media_files');
source.onmessage = (event) => {
    const data = JSON.parse(event.data);
    // Очищаем таблицу
    tbody.innerHTML = '';
    // data = { audio: [...], video: [...], images: [...] }
    for (const [type, files] of Object.entries(data)) {
        const list = files;
        list.forEach((fileName) => {
            const tr = document.createElement('tr');
            const tdType = document.createElement('td');
            tdType.textContent = type;
            const tdName = document.createElement('td');
            tdName.textContent = fileName;
            tr.appendChild(tdType);
            tr.appendChild(tdName);
            tbody.appendChild(tr);
        });
    }
};
//# sourceMappingURL=main.js.map