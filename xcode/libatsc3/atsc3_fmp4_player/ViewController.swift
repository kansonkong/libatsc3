//
//  ViewController.swift
//  atsc3_fmp4_player
//
//  Created by Jason Justman on 9/6/19.
//  Copyright © 2019 Jason Justman. All rights reserved.
//

import UIKit
import AVKit
import AVFoundation

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
    }

    @IBAction func playVideo(_ sender: Any) {
        guard let url = URL(string: "http://192.168.0.52:8000/master.m3u8") else {
            return
        }
        // Create an AVPlayer, passing it the HTTP Live Streaming URL.
        let player = AVPlayer(url: url)
        
        // Create a new AVPlayerViewController and pass it a reference to the player.
        let controller = AVPlayerViewController()
        controller.player = player
        
        // Modally present the player and call the player's play() method when complete.
        present(controller, animated: true) {
            player.play()
        }
    }    
}

