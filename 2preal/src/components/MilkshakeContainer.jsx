import React from 'react';
import MilkshakeCard from './MilkshakeCard';
import milkshakes from "../assets/milkshakes.json";
import "./MilkshakeContainer.css"
export const MilkshakeContainer = () => {
  return (
    <div className='Mcard-container'>
        
        <div className='milkshake-container-cards'>
        <div className='barra'></div>
            {
                milkshakes.map((milkshake)=>{
                        return <MilkshakeCard milkshake={milkshake} key={milkshake.id}/>
                })
            }

        </div>
    </div>
  )
}

