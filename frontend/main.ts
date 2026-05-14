const tbody = document.querySelector('#media-table tbody') as HTMLTableSectionElement;
const source = new EventSource('/media_files/stream');

source.onmessage = (event: MessageEvent) => {
    const data = JSON.parse(event.data);
    // Очищаем таблицу
    tbody.innerHTML = '';

    // data = { audio: [...], video: [...], images: [...] }
    for (const [type, files] of Object.entries(data)) {
        const list = files as string[];
        list.forEach((fileName: string) => {
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