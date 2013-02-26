
	c = xcb_connect(NULL,NULL);
	w = xcb_generate_id(c);
	xcb_create_window(c, s->root_depth, w, s->root,
	xcb_map_window(c, w);
	xcb_flush(c);
	while (!done && (e = xcb_wait_for_event(c))) {
