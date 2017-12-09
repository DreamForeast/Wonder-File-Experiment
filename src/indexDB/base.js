(function () {
    // IndexedDB
    var indexedDB = window.indexedDB || window.webkitIndexedDB || window.mozIndexedDB || window.OIndexedDB || window.msIndexedDB,
        IDBTransaction = window.IDBTransaction || window.webkitIDBTransaction || window.OIDBTransaction || window.msIDBTransaction,
        dbVersion = 1.0;

    var fileArray = ["base.css", "timg.jpg", "test.wd", "base.js"];
    // var fileArray = ["test.wd"];

    // Create/open database
    var request = indexedDB.open("elephantFiles", dbVersion),
        db,
        createObjectStore = function (dataBase) {
            // Create an objectStore
            console.log("Creating objectStore")
            dataBase.createObjectStore("elephants");
        },
        getFileByName = function (fileName) {
            var xhr = new XMLHttpRequest(),
                blob;

            xhr.open("GET", fileName, true);
            xhr.responseType = "blob";

            xhr.addEventListener("load", function () {
                if (xhr.status === 200) {
                    blob = xhr.response;

                    console.log(`${fileName} retrieved`);

                    putElephantInDb(fileName, blob);
                }
            }, false);

            xhr.send();
        },

        putElephantInDb = function (name, blob) {
            console.log(`Putting ${name} elephants in IndexedDB`);

            // Open a transaction to the database
            var readWriteMode = typeof IDBTransaction.READ_WRITE == "undefined" ? "readwrite" : IDBTransaction.READ_WRITE;
            transaction = db.transaction(["elephants"], readWriteMode);

            // Put the blob into the dabase
            transaction.objectStore("elephants").put(blob, name);
        };

    request.onerror = function (event) {
        console.log("Error creating/accessing IndexedDB database");
    };

    request.onsuccess = function (event) {
        console.log("Success creating/accessing IndexedDB database");
        db = request.result;

        db.onerror = function (event) {
            console.log("Error creating/accessing IndexedDB database");
        };

        // Interim solution for Google Chrome to create an objectStore. Will be deprecated
        if (db.setVersion) {
            if (db.version != dbVersion) {
                var setVersion = db.setVersion(dbVersion);
                setVersion.onsuccess = function () {
                    createObjectStore(db);
                };
            }
        }

        fileArray.forEach(function (fileItem) {
            getFileByName(fileItem);
        });

        var readWriteMode = typeof IDBTransaction.READ_WRITE == "undefined" ? "readwrite" : IDBTransaction.READ_WRITE;
        transaction = db.transaction(["elephants"], readWriteMode);
        var zip = new JSZip();

        var promiseArray = fileArray.map(function (fileItem) {
            return new Promise(function (resolve, reject) {
                var getFileFromIndexDB = transaction.objectStore("elephants").get(fileItem);
                getFileFromIndexDB.onsuccess = function (event) {
                    var binaryData = event.target.result;
                    console.log(`Got elephant! ${fileItem}`);

                    zip.file(fileItem, binaryData, { binary: true });

                    resolve();
                };
                getFileFromIndexDB.onerror = function (event) {
                    reject(`${fileItem} is get error`);
                };
            });
        });

        Promise.all(promiseArray).then(function () {
            zip.generateAsync({ type: "blob" })
                .then(function (content) {
                    saveAs(content, "example.zip");
                });
        }, function (err) {
            console.log(err);
        });

        // fileArray.forEach(function (fileItem) {
        //     transaction.objectStore("elephants").get(fileItem).onsuccess = function (event) {
        //         var binaryData = event.target.result;
        //         console.log(`Got elephant! ${fileItem}`);

        //         // var reader = new FileReader();

        //         // reader.onload = function (e) {
        //         //     var res = e.target.result;

        //         //     console.log(res);
        //         //     console.log(JSON.parse(res));
        //         // }

        //         // reader.readAsText(binaryData);

        //         zip.file(fileItem, binaryData, { binary: true });
        //     };
        // });


        document.getElementById("zipBtn").onclick = function () {
            zip.generateAsync({ type: "blob" })
                .then(function (content) {
                    saveAs(content, "example.zip");
                });
        };
    }

    // function showText() {
    // var imgElephant = document.getElementById("elephant");
    // var reader = new FileReader();

    // reader.onload = function (e) {
    //     imgElephant.innerHTML = e.target.result;
    // }
    // reader.readAsText(imgFile);
    // };

    // For future use. Currently only in latest Firefox versions
    request.onupgradeneeded = function (event) {
        createObjectStore(event.target.result);
    };
})();