#[doc = "

The layout task. Performs layout on the dom, builds display lists and sends
them to be rendered

"];

import task::*;
import comm::*;
import gfx::geom;
import gfx::renderer;
import dom::base::node;
import dom::rcu::scope;
import base::{btree, rd_tree_ops, wr_tree_ops, linked_subtree};
import base::block_layout_methods;
import dl = display_list;

enum msg {
    build(node),
    ping(chan<content::ping>),
    exit
}

fn layout(to_renderer: chan<renderer::msg>) -> chan<msg> {
    spawn_listener::<msg> { |po|
        loop {
            alt po.recv() {
              ping(ch) { ch.send(content::pong); }
              exit { break; }
              build(node) {
                #debug("layout: received layout request");
                let box = linked_subtree(node);
                box.reflow(geom::px_to_au(800));
                let dlist = build_display_list(box);
                to_renderer.send(renderer::render(dlist));
              }
            }
        }
    }
}

fn build_display_list(box: @base::box) -> display_list::display_list {
    let mut list = [box_to_display_item(box)];

    for btree.each_child(box) {|c|
        list += build_display_list(c);
    }

    #debug("display_list: %?", list);
    ret list;
}

fn box_to_display_item(box: @base::box) -> dl::display_item {
    let r = rand::rng();
    let item = dl::display_item({
        item_type: dl::solid_color(r.next() as u8,
                                   r.next() as u8,
                                   r.next() as u8),
        bounds: box.bounds
    });
    #debug("layout: display item: %?", item);
    ret item;
}
