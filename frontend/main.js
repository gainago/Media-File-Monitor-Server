var tbody = document.querySelector('#media-table tbody');
var source = new EventSource('/media_files');
source.onmessage = function (event) {
    var data = JSON.parse(event.data);
    // Очищаем таблицу
    tbody.innerHTML = '';
    var _loop_1 = function (type, files) {
        var list = files;
        list.forEach(function (fileName) {
            var tr = document.createElement('tr');
            var tdType = document.createElement('td');
            tdType.textContent = type;
            var tdName = document.createElement('td');
            tdName.textContent = fileName;
            tr.appendChild(tdType);
            tr.appendChild(tdName);
            tbody.appendChild(tr);
        });
    };
    // data = { audio: [...], video: [...], images: [...] }
    for (var _i = 0, _a = Object.entries(data); _i < _a.length; _i++) {
        var _b = _a[_i], type = _b[0], files = _b[1];
        _loop_1(type, files);
    }
};
