import React from 'react'
import "./MilkshakeCard.css"
export const MilkshakeCard = (props) => {
   const milkshake=props.milkshake
  return (
    <div className='card-container'>
        <img src="/static/images/imagen.jpg" alt=""/>
        <h3>{milkshake.name}</h3>
        <p>{milkshake.description}</p>
        <span>{milkshake.precio}</span>
        <button>Add</button>
    </div>
  );
}
export default  MilkshakeCard

