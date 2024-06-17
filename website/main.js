"use strict";
(function () {
    const menuBtn = document.querySelector("#menu");

    menuBtn.addEventListener("click", function () {
        this.ariaExpanded = !(menuBtn.ariaExpanded === "true");
    });

    
})();
