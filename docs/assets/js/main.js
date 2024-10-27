var sectionHeight = function() {
  var total    = $(window).height(),
      $section = $('section').css('height','auto');

  if ($section.outerHeight(true) < total) {
    var margin = $section.outerHeight(true) - $section.height();
    $section.height(total - margin - 20);
  } else {
    $section.css('height','auto');
  }
}

$(window).resize(sectionHeight);

$(function() {
  // 日本語ヘッダーに対応するために、番号からIDを算出しています。
  const id_header = "header_id_"; let i = 0;
  $("section h1, section h2, section h3").each(function(){
	const id = id_header + i++;
    $("nav ul").append("<li class='tag-" + this.nodeName.toLowerCase() + "'><a href='#" + id + "'>" + $(this).text() + "</a></li>");
    $(this).attr("id", id);
    $("nav ul li:first-child a").parent().addClass("active");
  });

  $("nav ul li").on("click", "a", function(event) {
    var position = $($(this).attr("href")).offset().top - 190;
    $("html, body").animate({scrollTop: position}, 400);
    $("nav ul li a").parent().removeClass("active");
    $(this).parent().addClass("active");
    event.preventDefault();
  });

  sectionHeight();

  $('img').on('load', sectionHeight);
});
